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

#define CUBEMAP_SIZE 1024

auto render_env_map() -> u32 {
  auto shader = Shader("../shaders/fullscreen.glsl",
                       "../shaders/panorama_to_cubemap.glsl");

  auto hdri = Texture<GL_TEXTURE_2D>("reading_room_1k.hdr");
  glEnableVertexAttribArray(0);

  u32 cubemap = 0;
  glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  for (u32 i = 0; i < 6; ++i) {
    fmt::println("defining {} texture", i);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F,
                 CUBEMAP_SIZE, CUBEMAP_SIZE, 0, GL_RGBA, GL_FLOAT, nullptr);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  u32 fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_CUBE_MAP_POSITIVE_X, cubemap, 0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    fmt::println("framebuffer status not complete\n");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  shader.bind();
  for (u32 i = 0; i < 6; ++i) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    u32 side = i;
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, cubemap, 0);

    glClearColor(0.5f, 0.5f, 0.5f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, CUBEMAP_SIZE, CUBEMAP_SIZE);

    hdri.bind();
    shader.upload_uniform_sampler("u_panorama", 0);
    shader.upload_uniform_int("u_currentFace", i32(i));

    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  glDeleteFramebuffers(1, &fbo);

  return cubemap;
}

Engine::Engine() : m_window(std::make_unique<Window>(width, height, "gi")) {
  if (glewInit() == GLEW_OK) {
    fmt::println("GL version: {}",
                 reinterpret_cast<const char*>(glGetString(GL_VERSION)));
  }
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(message_callback, 0);

  m_imgui_layer = std::make_unique<ImGuiLayer>(m_window->get_handle());

  // m_shader = std::make_shared<Shader>(vert, frag);
  // m_mesh = Mesh::from_obj("model.obj");

  m_shader = std::make_shared<Shader>("../shaders/cubemap_vert.glsl",
                                      "../shaders/cubemap_frag.glsl");
  m_mesh = Mesh::cube();
  m_camera = std::make_unique<Camera>(width, height, 90.0, 0.01, 100.0);
  m_renderer = std::make_unique<Renderer>(width, height);
}

auto Engine::run() -> void {
  m_cubemap = render_env_map();
  fmt::println("cubemap: {}", m_cubemap);
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
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);
    glActiveTexture(GL_TEXTURE0);
    m_shader->upload_uniform_int("u_cubemap", 0);
    m_shader->upload_uniform_mat4("modelViewProjection",
                                  m_camera->get_projection() *
                                      m_camera->get_view());

    m_renderer->draw();

    m_imgui_layer->end_frame();
  });
}
