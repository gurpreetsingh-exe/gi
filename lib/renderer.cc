#include <renderer.hh>

auto Renderer::upload_mesh(Mesh& mesh, ShaderRef shader) -> void {
  auto vao = VertexArray();
  vao.bind();
  vao.add_vertex_buffers({
      {
          .type = GL_FLOAT,
          .normalized = false,
          .stride = 12,
          .size = 3,
          .data = static_cast<void*>(mesh.vertices.data()),
          .byteSize = static_cast<isize>(mesh.vertices.size() * 12),
      },
  });
  vao.set_index_buffer(mesh.indices);
  vao.unbind();
  m_bindings.push_back({ std::move(vao), shader });
}

auto Renderer::resize(u32 width, u32 height) -> void {
  if (m_width != width || m_height != height) {
    m_width = width;
    m_height = height;
    m_msaa->resize(width, height);
    m_framebuffer->resize(width, height);
  }
}

auto Renderer::draw() -> void {
  m_msaa->bind();
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto& [vao, shader] : m_bindings) {
    shader->bind();
    vao.bind();
    glDrawElements(GL_TRIANGLES, vao.elems(), GL_UNSIGNED_INT, nullptr);
    glViewport(0, 0, i32(m_width), i32(m_height));
    vao.unbind();
    shader->unbind();
  }

  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msaa->get_id());
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer->get_id());
  glBlitFramebuffer(0, 0, i32(m_width), i32(m_height), 0, 0, i32(m_width),
                    i32(m_height), GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glDisable(GL_DEPTH_TEST);
  m_msaa->unbind();
}
