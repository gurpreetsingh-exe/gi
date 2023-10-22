#include <vertex_array.hh>

VertexArray::VertexArray()
    : m_vertex_buffers({}), m_index_buffer(std::make_unique<IndexBuffer>()) {
  glCreateVertexArrays(1, &m_Id);
  glBindVertexArray(m_Id);
}

VertexArray::VertexArray(VertexArray&& other) {
  m_Id = other.m_Id;
  m_vertex_buffers = std::move(other.m_vertex_buffers);
  m_index_buffer = std::move(other.m_index_buffer);
  m_elems = other.m_elems;
  other.m_Id = 0;
}

auto VertexArray::add_vertex_buffers(std::vector<BufferLayout> layouts)
    -> void {
  for (const auto& layout : layouts) {
    auto& buffer = m_vertex_buffers.emplace_back(layout.data, layout.byteSize);
    buffer.bind();
    auto id = static_cast<u32>(m_vertex_buffers.size() - 1);
    glEnableVertexAttribArray(id);
    glVertexAttribPointer(id, layout.size, layout.type, layout.normalized,
                          layout.stride, nullptr);
  }
}

void VertexArray::set_index_buffer(std::vector<u32>& indices) {
  m_index_buffer->upload_data(indices.data(),
                              static_cast<isize>(indices.size() * sizeof(u32)));
  m_elems = i32(indices.size());
  m_index_buffer->bind();
}
