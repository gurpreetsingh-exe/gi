// clang-format off
#include <GL/glew.h>
// clang-format on
#include <camera.hh>
#include <mesh.hh>
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
  std::unique_ptr<Window> m_Window;
  std::unique_ptr<Shader> m_Shader;
  std::unique_ptr<Mesh> m_Mesh;
  std::unique_ptr<VertexArray> m_VertexArray;
  std::unique_ptr<Camera> m_Camera;
};
