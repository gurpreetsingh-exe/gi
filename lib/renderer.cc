#include <renderer.hh>

#define CUBEMAP_SIZE 1024

auto Renderer::upload_mesh(std::unique_ptr<Mesh>&& mesh, Shader&& shader)
    -> void {
  m_imgui_layer->props.nvertices = mesh->vertices.size();
  auto vao = mesh_to_vao(std::move(mesh));
  m_bindings.push_back({ std::move(vao), std::move(shader) });
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

  for (auto& [vao, shader] : m_bindings) {
    shader.bind();
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);
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

  m_post_process_shader->bind();
  m_framebuffer->get_color_attachments()[0].bind();
  m_post_process_shader->upload_uniform_int("u_buf", i32(0));
  glDrawArrays(GL_TRIANGLES, 0, 3);

  m_final_fb->unbind();

  m_imgui_layer->end_frame();
}

auto Renderer::draw_cubemap() -> void {
  glDepthFunc(GL_LEQUAL);
  m_cubemap_shader->bind();
  m_cubemap_shader->upload_uniform_mat4("projection",
                                        m_camera->get_projection());
  m_cubemap_shader->upload_uniform_mat4("view", m_camera->get_view());
  m_cubemap_vao->bind();
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);
  m_cubemap_shader->upload_uniform_int("u_cubemap", 0);
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

auto Renderer::load_env_map(const fs::path& path) -> u32 {
  auto shader = Shader::quad("../shaders/panorama_to_cubemap.glsl");

  auto hdri = Texture<GL_TEXTURE_2D>(path);
  fmt::println("\"{}\" loaded\n  width: {}\n  height: {}", path.string(),
               hdri.get_width(), hdri.get_height());
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

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, CUBEMAP_SIZE, CUBEMAP_SIZE);

    hdri.bind();
    shader.upload_uniform_sampler("u_panorama", 0);
    shader.upload_uniform_int("u_currentFace", i32(i));

    glDrawArrays(GL_TRIANGLES, 0, 3);
    glViewport(0, 0, i32(m_width), i32(m_height));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  glDeleteFramebuffers(1, &fbo);

  return cubemap;
}
