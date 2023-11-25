#include <components/camera.hh>
#include <components/identifier.hh>
#include <editor.hh>

extern bool g_motion_blur;

Editor::Editor() : m_imgui_layer(std::make_unique<ImGuiLayer>()) {}

auto Editor::get_viewport_size() -> std::pair<u32, u32> {
  ImVec2 dim = m_imgui_layer->get_viewport_dimensions();
  return { u32(dim.x), u32(dim.y) };
}

auto Editor::draw_entity(Entity entity) -> void {
  auto flags = ((m_selection == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
               ImGuiTreeNodeFlags_OpenOnArrow |
               ImGuiTreeNodeFlags_FramePadding |
               ImGuiTreeNodeFlags_SpanAvailWidth;
  auto& component = entity.get_component<IdentifierComponent>();
  auto open = ImGui::TreeNodeEx(component.identifier, flags, "%s",
                                component.identifier);
  if (ImGui::IsItemClicked()) {
    m_selection = entity;
  }
  if (open) {
    ImGui::TreePop();
  }
}

auto Editor::draw_components(Entity entity) -> void {
  auto& name = entity.get_component<IdentifierComponent>().identifier;
  char buffer[64];
  std::memset(buffer, 0, sizeof(buffer));
  std::strncpy(buffer, name, sizeof(buffer));

  auto dim = ImGui::GetContentRegionAvail();
  ImGui::PushItemWidth(dim.x * 0.8f);
  if (ImGui::InputText("##", buffer, sizeof(buffer))) {
    std::memcpy(name, buffer, sizeof(buffer));
  }
  ImGui::PopItemWidth();

  ImGui::PushItemWidth(dim.x * 0.2f);
  ImGui::SameLine();
  if (ImGui::Button("Add")) {
    ImGui::OpenPopup("AddComponent");
  }
  ImGui::PopItemWidth();

  draw_component<CameraComponent>("Camera", m_selection, [](auto& component) {
    auto& camera = component.camera;
    auto fov = camera.get_fov();
    if (ImGui::SliderFloat("Fov", &fov, 10.0, 120.0)) {
      camera.set_fov(fov);
    }

    auto [near, far] = camera.get_clipping();
    f32 vec[] = { near, far };
    if (ImGui::DragFloat2("Clipping", (f32*)&vec, 0.01f, 0.001f, 10000.0f)) {
      camera.set_clipping(vec[0], vec[1]);
    }

    if (ImGui::IsItemActive()) {
      ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }
  });
}

auto Editor::update(const std::unique_ptr<Scene>& scene, const Framebuffer& fb)
    -> void {
  auto [w, h] = get_viewport_size();
  scene->resize(w, h);
  scene->update();
  m_imgui_layer->begin_frame();
  m_imgui_layer->update([&] {
    ImGui::Begin("Debug");
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Delta Time: %.3f ms", 1000.0f / io.Framerate);
    static auto vsync = false;
    if (ImGui::Checkbox("V-Sync", &vsync)) {
      glfwSwapInterval(vsync);
    }
    ImGui::Checkbox("Motion Blur", &g_motion_blur);
    ImGui::End();

    ImGui::Begin("Outliner");
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 2.f));
    for (auto& entity_id : scene->m_registry.view<IdentifierComponent>()) {
      draw_entity({ entity_id, scene.get() });
    }
    ImGui::PopStyleVar();
    if (ImGui::IsMouseDown(0) and ImGui::IsWindowHovered()) {
      m_selection = {};
    }

    if (ImGui::BeginPopupContextWindow("Add Menu",
                                       ImGuiPopupFlags_MouseButtonRight)) {
      if (ImGui::MenuItem("Empty")) {
        scene->create();
      }

      ImGui::EndPopup();
    }
    ImGui::End();

    ImGui::Begin("Properties");
    if (m_selection) {
      draw_components(m_selection);
    }
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");

    auto dim = ImGui::GetContentRegionAvail();
    auto& tex = fb.get_color_attachments()[0];
    ImGui::Image((void*)(intptr_t)tex.get_id(), dim, ImVec2 { 0.0, 0.0 },
                 ImVec2 { 1.0, -1.0 });

    ImGui::End();
    ImGui::PopStyleVar();

    return dim;
  });
  m_imgui_layer->end_frame();
}
