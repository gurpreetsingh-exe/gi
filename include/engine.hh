#include <shader.hh>
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
};
