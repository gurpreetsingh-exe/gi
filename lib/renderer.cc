#include <renderer.hh>

#define CUBEMAP_SIZE 1024

Renderer::Renderer(std::shared_ptr<ResourceManager> rm) {
  auto [w, h] = window.get_size();
  m_width = w;
  m_height = h;
  m_resource_manager = rm;
  m_camera = std::make_unique<Camera>(w, h, 70.0, 0.01, 100.0);
  m_msaa = std::make_unique<Framebuffer<GL_TEXTURE_2D_MULTISAMPLE>>(w, h);
  m_framebuffer = std::make_unique<Framebuffer<GL_TEXTURE_2D>>(w, h);
  m_final_fb = std::make_unique<Framebuffer<GL_TEXTURE_2D>>(w, h);
  m_cubemap = std::make_unique<CubeMap>(
      CubeMap::cubemap("industrial_sunset_puresky_2k.hdr"));
  m_cubemap_vao = std::make_unique<VertexArray>(mesh_to_vao(Mesh::cube()));
  m_cubemap_shader = m_resource_manager->load_shader(
      "../shaders/cubemap_vert.glsl", "../shaders/cubemap_frag.glsl");
  m_imgui_layer = std::make_unique<ImGuiLayer>();
  m_post_process_shader =
      m_resource_manager->load_shader(Shader::quad("../shaders/post.glsl"));
}

auto Renderer::upload_mesh(std::unique_ptr<Mesh>&& mesh,
                           Resource<Shader> shader) -> void {
  m_imgui_layer->props.nvertices = mesh->vertices.size();
  auto vao = mesh_to_vao(std::move(mesh));
  m_bindings.push_back({ std::move(vao), shader });
}

auto Renderer::resize(u32 width, u32 height) -> void {
  if (m_width != width || m_height != height) {
    m_width = width;
    m_height = height;
    m_msaa->resize(width, height);
    m_framebuffer->resize(width, height);
    m_final_fb->resize(width, height);
    m_camera->resize(f32(width), f32(height));
  }
}

auto Renderer::update() -> void {
  m_camera->set_fov(m_imgui_layer->props.camera_fov);
  m_camera->set_clipping(m_imgui_layer->props.clip_near,
                         m_imgui_layer->props.clip_far);
  m_camera->update();
  m_imgui_layer->begin_frame();
  m_imgui_layer->update(framebuffer());
  ImVec2 dim = m_imgui_layer->get_viewport_dimensions();
  resize(u32(dim.x), u32(dim.y));
}

auto Renderer::draw() -> void {
  m_msaa->bind();
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto& [vao, s] : m_bindings) {
    auto& shader = m_resource_manager->get_shader(s);
    shader.bind();
    m_cubemap->bind();
    shader.upload_uniform_int("u_cubemap", 0);
    shader.upload_uniform_mat4("projection", m_camera->get_projection());
    shader.upload_uniform_mat4("view", m_camera->get_view());
    vao.bind();
    glDrawElements(GL_TRIANGLES, vao.elems(), GL_UNSIGNED_INT, nullptr);
    glViewport(0, 0, i32(m_width), i32(m_height));
    vao.unbind();
    shader.unbind();
  }

  draw_cubemap();

  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msaa->get_id());
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer->get_id());
  glBlitFramebuffer(0, 0, i32(m_width), i32(m_height), 0, 0, i32(m_width),
                    i32(m_height), GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glDisable(GL_DEPTH_TEST);
  m_msaa->unbind();

  m_final_fb->bind();

  auto& shader = m_resource_manager->get_shader(m_post_process_shader);
  shader.bind();
  m_framebuffer->get_color_attachments()[0].bind();
  shader.upload_uniform_int("u_buf", i32(0));
  glDrawArrays(GL_TRIANGLES, 0, 3);

  m_final_fb->unbind();

  m_imgui_layer->end_frame();
}

auto Renderer::draw_cubemap() -> void {
  glDepthFunc(GL_LEQUAL);
  auto& shader = m_resource_manager->get_shader(m_cubemap_shader);
  shader.bind();
  shader.upload_uniform_mat4("projection",
                                        m_camera->get_projection());
  shader.upload_uniform_mat4("view", m_camera->get_view());
  m_cubemap_vao->bind();
  m_cubemap->bind();
  shader.upload_uniform_int("u_cubemap", 0);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
  m_cubemap_vao->unbind();
  glDepthFunc(GL_LESS);
}

auto Renderer::mesh_to_vao(std::unique_ptr<Mesh>&& mesh) -> VertexArray {
  auto vao = VertexArray();
  vao.bind();
  vao.add_vertex_buffers({
      {
          .type = GL_FLOAT,
          .normalized = false,
          .stride = 12,
          .size = 3,
          .data = static_cast<void*>(mesh->vertices.data()),
          .byteSize = static_cast<isize>(mesh->vertices.size() * 12),
      },
  });
  vao.set_index_buffer(mesh->indices);
  vao.unbind();
  return vao;
}
