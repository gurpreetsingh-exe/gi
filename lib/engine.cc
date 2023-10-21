#include <engine.hh>

static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
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

Engine::Engine() : m_Window(std::make_unique<Window>(width, height, "gi")) {
  if (glewInit() == GLEW_OK) {
    fmt::println("GL version: {}",
                 reinterpret_cast<const char*>(glGetString(GL_VERSION)));
  }
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  m_Shader = std::make_unique<Shader>(vert, frag);
  m_Mesh = Mesh::from_obj("model.obj");
  m_VertexArray = std::make_unique<VertexArray>();
  m_Camera = std::make_unique<Camera>(width, height, 90.0, 0.01, 100.0);
}

auto Engine::run() -> void {
  m_Shader->bind();
  m_VertexArray->bind();
  auto layout = std::vector<BufferLayout> {
    {
        .type = GL_FLOAT,
        .normalized = false,
        .stride = 12,
        .size = 3,
        .data = static_cast<void*>(m_Mesh->vertices.data()),
        .byteSize = static_cast<isize>(m_Mesh->vertices.size() * 12),
    },
  };
  m_VertexArray->add_vertex_buffers(std::move(layout));
  m_VertexArray->set_index_buffer(m_Mesh->indices);

  m_Window->is_running([&] {
    auto event = m_Window->get_event();
    m_Camera->update(m_Window->get_handle(), event);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    m_Shader->uploadUniformMat4(
        "modelViewProjection", m_Camera->getProjection() * m_Camera->getView());
    m_VertexArray->bind();
    auto elems = static_cast<i32>(m_Mesh->indices.size());
    glDrawElements(GL_TRIANGLES, elems, GL_UNSIGNED_INT, nullptr);
  });
}
