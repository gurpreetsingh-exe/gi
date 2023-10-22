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
  m_cubemap = render_env_map();
  // m_window->is_running([&] { render_env_map(); });
  // std::exit(0);

  m_imgui_layer = std::make_unique<ImGuiLayer>(m_window->get_handle());
  m_shader = std::make_unique<Shader>(vert, frag);
  m_mesh = Mesh::from_obj("model.obj");

  m_msaa =
      std::make_unique<Framebuffer<GL_TEXTURE_2D_MULTISAMPLE>>(width, height);
  m_msaa->bind();
  if (!m_msaa->is_complete()) {
    eprint("multisampled framebuffer setup not completed");
  }
  m_msaa->unbind();

  m_framebuffer = std::make_unique<Framebuffer<GL_TEXTURE_2D>>(width, height);
  m_framebuffer->bind();
  if (!m_framebuffer->is_complete()) {
    eprint("framebuffer setup not completed");
  }
  m_framebuffer->unbind();

  m_vertex_array = std::make_unique<VertexArray>();
  m_camera = std::make_unique<Camera>(width, height, 90.0, 0.01, 100.0);
}

auto Engine::run() -> void {
  std::vector<Vertex> vertices = {
    { -1, -1, -1 }, { 1, -1, -1 }, { 1, 1, -1 }, { -1, 1, -1 },
    { -1, -1, 1 },  { 1, -1, 1 },  { 1, 1, 1 },  { -1, 1, 1 },
  };

  std::vector<u32> indices = { 1, 2, 0, 2, 3, 0, 6, 2, 1, 1, 5, 6,
                               6, 5, 4, 4, 7, 6, 6, 3, 2, 7, 3, 6,
                               3, 7, 0, 7, 4, 0, 5, 1, 0, 4, 5, 0 };

  auto cubemap_shader =
      Shader("../shaders/cubemap_vert.glsl", "../shaders/cubemap_frag.glsl");
  auto cubemap_vao = VertexArray();
  cubemap_vao.bind();
  cubemap_vao.add_vertex_buffers({
      {
          .type = GL_FLOAT,
          .normalized = false,
          .stride = 12,
          .size = 3,
          .data = static_cast<void*>(vertices.data()),
          .byteSize = static_cast<isize>(vertices.size() * 12),
      },
  });
  cubemap_vao.set_index_buffer(indices);

  m_vertex_array->bind();
  auto layout = std::vector<BufferLayout> {
    {
        .type = GL_FLOAT,
        .normalized = false,
        .stride = 12,
        .size = 3,
        .data = static_cast<void*>(m_mesh->vertices.data()),
        .byteSize = static_cast<isize>(m_mesh->vertices.size() * 12),
    },
  };
  m_vertex_array->add_vertex_buffers(std::move(layout));
  m_vertex_array->set_index_buffer(m_mesh->indices);
  m_vertex_array->unbind();

  m_window->is_running([&] {
    auto event = m_window->get_event();
    m_camera->update(m_window->get_handle(), event);

    m_imgui_layer->begin_frame();
    m_imgui_layer->update(*m_framebuffer);

    ImVec2 dim = m_imgui_layer->get_viewport_dimensions();
    m_msaa->resize(u32(dim.x), u32(dim.y));
    m_framebuffer->resize(u32(dim.x), u32(dim.y));
    m_camera->resize(dim.x, dim.y);

    m_msaa->bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader->bind();
    m_shader->upload_uniform_mat4("modelViewProjection",
                                  m_camera->get_projection() *
                                      m_camera->get_view());
    m_vertex_array->bind();
    auto elems = static_cast<i32>(m_mesh->indices.size());
    glDrawElements(GL_TRIANGLES, elems, GL_UNSIGNED_INT, nullptr);
    glViewport(0, 0, i32(dim.x), i32(dim.y));
    m_vertex_array->unbind();
    // glDisable(GL_DEPTH_TEST);

    cubemap_shader.bind();
    cubemap_shader.upload_uniform_mat4("modelViewProjection",
                                       m_camera->get_projection() *
                                           m_camera->get_view());
    // cubemap_shader.upload_uniform_sampler("u_GGXEnvSampler", i32(m_cubemap));
    cubemap_vao.bind();
    elems = static_cast<i32>(indices.size());
    glDrawElements(GL_TRIANGLES, elems, GL_UNSIGNED_INT, nullptr);
    // glViewport(0, 0, i32(dim.x), i32(dim.y));
    cubemap_vao.unbind();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msaa->get_id());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer->get_id());
    glBlitFramebuffer(0, 0, i32(dim.x), i32(dim.y), 0, 0, i32(dim.x),
                      i32(dim.y), GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glDisable(GL_DEPTH_TEST);
    m_msaa->unbind();

    m_imgui_layer->end_frame();
  });
}
