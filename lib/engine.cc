// clang-format off
#include <GL/glew.h>
// clang-format on
#include <engine.hh>

Engine::Engine() : m_Window(std::make_unique<Window>(width, height, "gi")) {
  if (glewInit() == GLEW_OK) {
    fmt::println("GL version: {}",
                 reinterpret_cast<const char*>(glGetString(GL_VERSION)));
  }
  m_Shader = std::make_unique<Shader>(vert, frag);
}

auto Engine::run() -> void {
  m_Window->is_running([] {});
}
