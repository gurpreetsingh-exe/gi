#include <engine.hh>

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

Engine::Engine() : m_window(std::make_unique<Window>(width, height, "gi")) {
  if (glewInit() == GLEW_OK) {
    fmt::println("GL version: {}",
                 reinterpret_cast<const char*>(glGetString(GL_VERSION)));
  }
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(message_callback, 0);

  m_imgui_layer = std::make_unique<ImGuiLayer>(m_window->get_handle());

  m_shader = std::make_shared<Shader>(vert, frag);
  m_mesh = Mesh::from_obj("model1.obj");
  m_renderer = std::make_unique<Renderer>(width, height);
}

auto Engine::run() -> void {
  m_renderer->upload_mesh(*m_mesh, m_shader);

  m_window->is_running([&] {
    m_renderer->update(m_window);
    m_imgui_layer->begin_frame();
    m_imgui_layer->update(*m_renderer->framebuffer());
    ImVec2 dim = m_imgui_layer->get_viewport_dimensions();
    m_renderer->resize(u32(dim.x), u32(dim.y));
    m_renderer->draw();
    m_imgui_layer->end_frame();
  });
}
