// clang-format off
#include <GL/glew.h>
// clang-format on
#include <framebuffer.hh>
#include <imgui_layer.hh>
#include <mesh.hh>
#include <renderer.hh>
#include <shader.hh>
#include <vertex_array.hh>
#include <window.hh>

class Engine {
  static constexpr u32 width = 600;
  static constexpr u32 height = 600;
  std::string vert = "../shaders/vertex.glsl";
  std::string frag = "../shaders/fragment.glsl";

public:
  Engine();

public:
  auto run() -> void;

private:
  std::unique_ptr<Window> m_window;
  std::shared_ptr<Shader> m_shader;
  std::unique_ptr<Mesh> m_mesh;
  std::unique_ptr<ImGuiLayer> m_imgui_layer;
  std::unique_ptr<Renderer> m_renderer;
};
