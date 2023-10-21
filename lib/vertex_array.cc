#include <vertex_array.hh>

VertexArray::VertexArray() {
  glCreateVertexArrays(1, &m_Id);
  glBindVertexArray(m_Id);
}

auto VertexArray::add_vertex_buffers(std::vector<BufferLayout> layouts)
    -> void {
  for (const auto& layout : layouts) {
    auto& buffer = m_Vertex_buffers.emplace_back(layout.data, layout.byteSize);
    buffer.bind();
    auto id = static_cast<u32>(m_Vertex_buffers.size() - 1);
    glEnableVertexAttribArray(id);
    glVertexAttribPointer(id, layout.size, layout.type, layout.normalized,
                          layout.stride, nullptr);
  }
}

void VertexArray::set_index_buffer(std::vector<u32>& indices) {
  m_Index_buffer = std::make_unique<Buffer<GL_ELEMENT_ARRAY_BUFFER>>(
      indices.data(), static_cast<isize>(indices.size() * sizeof(u32)));
  m_Index_buffer->bind();
}
