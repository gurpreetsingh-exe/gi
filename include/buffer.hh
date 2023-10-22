#ifndef BUFFER_H
#define BUFFER_H

// clang-format off
#include <GL/glew.h>
// clang-format on

#include <utils.hh>

template <u32 target>
class Buffer {
public:
  Buffer() {
    glCreateBuffers(1, &m_Id);
    glBindBuffer(target, m_Id);
  }

  Buffer(void* data, isize size) {
    glCreateBuffers(1, &m_Id);
    glBindBuffer(target, m_Id);
    glBufferData(target, size, data, GL_STATIC_DRAW);
  }

  ~Buffer() { glDeleteBuffers(1, &m_Id); }

public:
  auto bind() -> void { glBindBuffer(target, m_Id); }
  auto unbind() -> void { glBindBuffer(target, 0); }

  auto upload_data(void* data, isize size) -> void {
    glBindBuffer(target, m_Id);
    glBufferData(target, size, data, GL_STATIC_DRAW);
  }

private:
  u32 m_Id;
};

#endif // !BUFFER_H
