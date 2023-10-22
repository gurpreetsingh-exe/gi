#include <engine.hh>
#include <env_renderer.hh>

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
  // m_cubemap = render_env_map();

  m_imgui_layer = std::make_unique<ImGuiLayer>(m_window->get_handle());
  m_shader = std::make_shared<Shader>(vert, frag);
  m_mesh = Mesh::from_obj("model.obj");
  m_camera = std::make_unique<Camera>(width, height, 90.0, 0.01, 100.0);
  m_renderer = std::make_unique<Renderer>(width, height);
}

auto Engine::run() -> void {
  m_renderer->upload_mesh(*m_mesh, m_shader);

  m_window->is_running([&] {
    auto event = m_window->get_event();
    m_camera->update(m_window->get_handle(), event);

    m_imgui_layer->begin_frame();
    m_imgui_layer->update(*m_renderer->framebuffer());
    ImVec2 dim = m_imgui_layer->get_viewport_dimensions();
    m_renderer->resize(u32(dim.x), u32(dim.y));
    m_camera->resize(dim.x, dim.y);

    m_shader->bind();
    m_shader->upload_uniform_mat4("modelViewProjection",
                                  m_camera->get_projection() *
                                      m_camera->get_view());
    m_renderer->draw();

    m_imgui_layer->end_frame();
  });
}
