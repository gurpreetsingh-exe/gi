#ifndef BUFFER_H
#define BUFFER_H

// clang-format off
#include <GL/glew.h>
// clang-format on

#include <utils.hh>

template <u32 target>
class Buffer {
public:
  Buffer() = default;

  Buffer(void* data, isize size) {
    glCreateBuffers(1, &m_Id);
    glBindBuffer(target, m_Id);
    glBufferData(target, size, data, GL_STATIC_DRAW);
  }

  ~Buffer() { glDeleteBuffers(1, &m_Id); }

public:
  auto bind() -> void { glBindBuffer(target, m_Id); }
  auto unbind() -> void { glBindBuffer(target, 0); }

private:
  u32 m_Id;
};

#endif // !BUFFER_H
