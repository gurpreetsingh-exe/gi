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
  fprintf(stderr, "%stype = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "error: " : ""), type, severity,
          message);
}

Engine::Engine() {
  if (glewInit() == GLEW_OK) {
    fmt::println("GL version: {}",
                 reinterpret_cast<const char*>(glGetString(GL_VERSION)));
  }
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(message_callback, 0);
  m_resource_manager = std::make_shared<ResourceManager>();
  m_compositor = std::make_unique<Compositor>(m_resource_manager);
  m_renderer = std::make_unique<Renderer>(m_resource_manager);
  m_scene = std::make_unique<Scene>();
  m_editor = std::make_unique<Editor>();
}

auto Engine::run() -> void {
  auto shader = m_resource_manager->create<Shader>(vert, frag);
  m_resource_manager->get(shader).upload_binding("Camera", 0);
  m_renderer->upload_mesh(Mesh::from_obj("model1.obj"), shader);
  m_renderer->set_active_camera(m_scene->add_camera());
  m_compositor->add_effect("../shaders/tonemap.glsl");

  m_editor->update(m_scene, m_renderer->framebuffer());
  window.is_running([&] {
    auto [x, y] = m_editor->get_viewport_size();
    m_renderer->resize(x, y);
    m_renderer->draw();
    auto& texture = m_renderer->framebuffer().get_color_attachments()[0];
    m_compositor->resize(x, y);
    m_compositor->composite(texture);
    m_editor->update(m_scene, m_compositor->framebuffer());
  });
}
