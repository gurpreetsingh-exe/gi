#include <components/camera.hh>
#include <renderer.hh>

#define CUBEMAP_SIZE 1024

Renderer::Renderer(std::shared_ptr<ResourceManager> rm) {
  auto [w, h] = window.get_size();
  m_width = w;
  m_height = h;
  m_resource_manager = rm;
  Framebuffer::Desc desc = { .width = i32(w),
                             .height = i32(h),
                             .multisampled = true };
  m_msaa = m_resource_manager->create<Framebuffer>(desc);
  desc.multisampled = false;
  m_framebuffer = m_resource_manager->create<Framebuffer>(desc);
  m_final_fb = m_resource_manager->create<Framebuffer>(desc);
  m_cubemap = m_resource_manager->create<Texture>(
      Texture::Type::TextureCube, "industrial_sunset_puresky_2k.hdr");
  m_cubemap_vao = std::make_unique<VertexArray>(mesh_to_vao(Mesh::cube()));
  m_cubemap_shader = m_resource_manager->create<Shader>(
      "../shaders/cubemap_vert.glsl", "../shaders/cubemap_frag.glsl");
  m_post_process_shader =
      m_resource_manager->create<Shader>(Shader::quad("../shaders/post.glsl"));
}

auto Renderer::upload_mesh(std::unique_ptr<Mesh>&& mesh,
                           Resource<Shader> shader) -> void {
  auto vao = mesh_to_vao(std::move(mesh));
  m_bindings.push_back({ std::move(vao), shader });
}

auto Renderer::resize(u32 width, u32 height) -> void {
  if (m_width != width || m_height != height) {
    m_width = width;
    m_height = height;
    m_resource_manager->get(m_msaa).resize(width, height);
    m_resource_manager->get(m_framebuffer).resize(width, height);
    m_resource_manager->get(m_final_fb).resize(width, height);
    auto& camera = m_camera.get_component<CameraComponent>().camera;
    camera.resize(f32(width), f32(height));
  }
}

auto Renderer::draw() -> void {
  auto& msaa = m_resource_manager->get(m_msaa);
  auto& framebuffer = m_resource_manager->get(m_framebuffer);
  msaa.bind();
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto& camera = m_camera.get_component<CameraComponent>().camera;

  for (auto& [vao, s] : m_bindings) {
    auto& shader = m_resource_manager->get<Shader>(s);
    shader.bind();
    m_resource_manager->bind(m_cubemap);
    shader.upload_uniform_int("u_cubemap", 0);
    shader.upload_uniform_mat4("projection", camera.get_projection());
    shader.upload_uniform_mat4("view", camera.get_view());
    vao.bind();
    glDrawElements(GL_TRIANGLES, vao.elems(), GL_UNSIGNED_INT, nullptr);
    glViewport(0, 0, i32(m_width), i32(m_height));
    vao.unbind();
    shader.unbind();
  }

  draw_cubemap(camera);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, msaa.get_id());
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.get_id());
  glBlitFramebuffer(0, 0, i32(m_width), i32(m_height), 0, 0, i32(m_width),
                    i32(m_height), GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glDisable(GL_DEPTH_TEST);
  msaa.unbind();

  m_resource_manager->bind(m_final_fb);
  auto& shader = m_resource_manager->get<Shader>(m_post_process_shader);
  shader.bind();
  framebuffer.get_color_attachments()[0].bind();
  shader.upload_uniform_int("u_buf", i32(0));
  glDrawArrays(GL_TRIANGLES, 0, 3);
  m_resource_manager->unbind(m_final_fb);
}

auto Renderer::draw_cubemap(Camera& camera) -> void {
  glDepthFunc(GL_LEQUAL);
  auto& shader = m_resource_manager->get<Shader>(m_cubemap_shader);
  shader.bind();
  shader.upload_uniform_mat4("projection", camera.get_projection());
  shader.upload_uniform_mat4("view", camera.get_view());
  m_cubemap_vao->bind();
  m_resource_manager->bind(m_cubemap);
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
          .byteSize = isize(mesh->vertices.size() * 12),
      },
  });
  vao.set_index_buffer(mesh->indices);
  vao.unbind();
  return vao;
}
