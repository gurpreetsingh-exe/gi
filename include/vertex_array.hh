#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H
// clang-format off
#include <GL/glew.h>
// clang-format on

#include "buffer.hh"

struct BufferLayout {
  uint32_t type;
  bool normalized;
  i32 stride;
  i32 size;
  void* data;
  isize byteSize;
};

class VertexArray {
public:
  VertexArray();
  ~VertexArray() { glDeleteVertexArrays(1, &m_Id); }

public:
  auto bind() -> void { glBindVertexArray(m_Id); }
  auto unbind() -> void { glBindVertexArray(0); }
  auto add_vertex_buffers(std::vector<BufferLayout> layouts) -> void;
  auto set_index_buffer(std::vector<u32>& indices) -> void;

private:
  uint32_t m_Id;
  std::vector<Buffer<GL_ARRAY_BUFFER>> m_Vertex_buffers;
  std::unique_ptr<Buffer<GL_ELEMENT_ARRAY_BUFFER>> m_Index_buffer;
};

#endif // !VERTEX_ARRAY_H
