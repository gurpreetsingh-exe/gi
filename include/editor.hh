#ifndef EDITOR_H
#define EDITOR_H

// clang-format off
#include <framebuffer.hh>
// clang-format on
#include <entity.hh>
#include <imgui_layer.hh>
#include <scene.hh>
#include <utils.hh>

class Editor {
public:
  Editor();
  ~Editor() = default;

public:
  auto update(const std::unique_ptr<Scene>& scene, const Framebuffer& fb)
      -> void;
  auto get_viewport_size() -> std::pair<u32, u32>;

private:
  auto draw_entity(Entity entity) -> void;
  auto draw_component(Entity entity) -> void;
  auto draw_components(Entity entity) -> void;

  template <typename T, typename DrawFunc>
  auto draw_component(const std::string& name, Entity entity, DrawFunc draw)
      -> void {
    if (not entity)
      return;
    if (not entity.has_component<T>())
      return;

    auto& component = entity.get_component<T>();
    auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                 ImGuiTreeNodeFlags_SpanAvailWidth |
                 ImGuiTreeNodeFlags_FramePadding;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 { 0.f, 2.f });
    auto open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, "%s",
                                  name.c_str());
    ImGui::PopStyleVar();

    if (open) {
      draw(component);
      ImGui::TreePop();
    }

    ImGui::Separator();
  }

private:
  std::unique_ptr<ImGuiLayer> m_imgui_layer;
  Entity m_selection;
};

#endif // !EDITOR_H
