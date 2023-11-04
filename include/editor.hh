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

private:
  std::unique_ptr<ImGuiLayer> m_imgui_layer;
  Entity m_selection;
};

#endif // !EDITOR_H
