#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include <framebuffer.hh>
#include <imgui.h>
#include <window.hh>

class ImGuiLayer {
public:
  ImGuiLayer();
  ~ImGuiLayer();

public:
  template <typename Callable>
  auto update(Callable cb) -> void {
    m_dim = cb();
  }

  void begin_frame();
  void end_frame();
  ImVec2 get_viewport_dimensions() { return m_dim; }

private:
  bool m_open;
  bool m_opt_fullscreen = true;
  bool m_opt_padding = false;
  ImGuiDockNodeFlags m_dockspace_flags = ImGuiDockNodeFlags_None;
  ImGuiWindowFlags m_window_flags =
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  ImVec2 m_dim;
};

#endif // !IMGUI_LAYER_H
