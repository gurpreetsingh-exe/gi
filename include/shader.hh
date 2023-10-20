#ifndef SHADER_H
#define SHADER_H

// clang-format off
#include <GL/glew.h>
// clang-format on
#include <glm/glm.hpp>
#include <utils.hh>

class Shader {
public:
  Shader(const std::string& vert_src, const std::string& frag_src);
  ~Shader() { glDeleteProgram(m_Id); }

public:
  auto bind() -> void { glUseProgram(m_Id); };
  auto unbind() -> void { glUseProgram(0); };
  auto get_id() const -> u32 { return m_Id; }

private:
  u32 m_Id;
};

#endif // !SHADER_H
