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
  Shader(Shader&& other);
  ~Shader() {
    dbg("~Shader(%d)\n", m_Id);
    glDeleteProgram(m_Id);
  }

public:
  static auto quad(const std::string& frag_src) -> Shader;
  auto bind() -> void { glUseProgram(m_Id); };
  auto unbind() -> void { glUseProgram(0); };
  auto get_id() const -> u32 { return m_Id; }
  auto upload_uniform_int(const std::string& name, i32 value) -> void;
  auto upload_uniform_sampler(const std::string& name, i32 tex) -> void;
  auto upload_uniform_mat4(const std::string& name, const glm::mat4& mat)
      -> void;

private:
  u32 m_Id;
};

#endif // !SHADER_H
