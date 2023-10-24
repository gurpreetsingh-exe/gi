#include <engine.hh>
#include <mesh.hh>
#include <shader.hh>

Window window = Window(600, 400, "gi");

static void GLAPIENTRY message_callback(GLenum source, GLenum type, GLuint id,
                                        GLenum severity, GLsizei length,
                                        const GLchar* message,
                                        const void* userParam) {
  (void)source;
  (void)id;
  (void)length;
  (void)userParam;
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}

Engine::Engine() {
  if (glewInit() == GLEW_OK) {
    fmt::println("GL version: {}",
                 reinterpret_cast<const char*>(glGetString(GL_VERSION)));
  }
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(message_callback, 0);
  m_renderer = std::make_unique<Renderer>();
}

auto Engine::run() -> void {
  m_renderer->upload_mesh(std::move(Mesh::from_obj("model1.obj")),
                          Shader(vert, frag));

  window.is_running([&] {
    m_renderer->update();
    m_renderer->draw();
  });
}
