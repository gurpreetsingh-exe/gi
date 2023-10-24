#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include <framebuffer.hh>
#include <imgui.h>
#include <window.hh>

struct Properties {
  f32 camera_fov = 90.0f;
  f32 clip_near = 0.01f;
  f32 clip_far = 100.0f;
  usize nvertices = 0;
  bool reload_shaders = false;
};

class ImGuiLayer {
public:
  ImGuiLayer(GLFWwindow* window);
  ~ImGuiLayer();

public:
  Properties props = {};

public:
  void update();
  void update(Framebuffer<GL_TEXTURE_2D>* framebuffer);
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
