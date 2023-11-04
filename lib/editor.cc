#include <components/camera.hh>
#include <components/identifier.hh>
#include <editor.hh>

Editor::Editor() : m_imgui_layer(std::make_unique<ImGuiLayer>()) {}

auto Editor::get_viewport_size() -> std::pair<u32, u32> {
  ImVec2 dim = m_imgui_layer->get_viewport_dimensions();
  return { u32(dim.x), u32(dim.y) };
}

auto Editor::draw_entity(Entity entity) -> void {
  auto flags = ((m_selection == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
               ImGuiTreeNodeFlags_OpenOnArrow |
               ImGuiTreeNodeFlags_SpanAvailWidth;
  auto component = entity.get_component<IdentifierComponent>();
  auto ptr = reinterpret_cast<void*>(&entity);
  auto opened = ImGui::TreeNodeEx(ptr, flags, "%s", component.identifier);
  if (ImGui::IsItemClicked()) {
    m_selection = entity;
  }
  (void)opened;
}

auto Editor::draw_component(Entity entity) -> void {
  if (not entity)
    return;
  ImGui::Text("Camera");
  auto& component = entity.get_component<CameraComponent>();
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
}

auto Editor::update(const std::unique_ptr<Scene>& scene, const Framebuffer& fb)
    -> void {
  auto [w, h] = get_viewport_size();
  scene->resize(w, h);
  scene->update();
  m_imgui_layer->begin_frame();
  m_imgui_layer->update([&](auto& props) {
    ImGui::Begin("Debug");
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Delta Time: %.3f ms", 1000.0f / io.Framerate);
    ImGui::Text("Vertices: %zu", props.nvertices);
    props.reload_shaders = ImGui::Button("Reload Shaders");
    ImGui::End();

    ImGui::Begin("Outliner");
    auto entities = scene->m_registry.view<IdentifierComponent>();
    for (usize i = 0; i < entities.size(); ++i) {
      Entity entity = { entities[i], scene.get() };
      // auto component = entity.get_component<IdentifierComponent>();
      // auto selected = entity == m_selection;
      // ImGui::Selectable(component.identifier, &selected);
      draw_entity(entity);
    }
    ImGui::End();

    ImGui::Begin("Properties");
    draw_component(m_selection);
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");

    auto dim = ImGui::GetContentRegionAvail();
    ImGui::Image((void*)(intptr_t)fb.get_color_attachments()[0].get_id(), dim,
                 ImVec2 { 0.0, 0.0 }, ImVec2 { 1.0, -1.0 });

    ImGui::End();
    ImGui::PopStyleVar();

    return dim;
  });
  m_imgui_layer->end_frame();
}
