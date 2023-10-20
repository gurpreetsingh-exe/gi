// clang-format off
#include <GL/glew.h>
// clang-format on
#include <glm/gtc/type_ptr.hpp>
#include <shader.hh>

Shader::Shader(const std::string& vert_path, const std::string& frag_path) {
  auto vert_src = read_file(vert_path);
  auto frag_src = read_file(frag_path);

  auto vert = vert_src.c_str();
  auto frag = frag_src.c_str();

  auto vert_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_shader, 1, &vert, 0);
  glCompileShader(vert_shader);

  i32 result;
  glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    i32 length;
    glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &length);
    auto info_log = std::vector<char>(static_cast<usize>(length));
    glGetShaderInfoLog(vert_shader, length, &length, &info_log[0]);
    glDeleteShader(vert_shader);
    eprint("%s\n", info_log.data());
  }

  uint32_t frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader, 1, &frag, 0);
  glCompileShader(frag_shader);

  glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    i32 length;
    glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &length);
    auto info_log = std::vector<char>(static_cast<usize>(length));
    glGetShaderInfoLog(frag_shader, length, &length, &info_log[0]);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    eprint("%s\n", info_log.data());
  }

  m_Id = glCreateProgram();
  glAttachShader(m_Id, vert_shader);
  glAttachShader(m_Id, frag_shader);
  glLinkProgram(m_Id);

  auto is_linked = 0;
  glGetProgramiv(m_Id, GL_LINK_STATUS, reinterpret_cast<i32*>(is_linked));
  if (is_linked == GL_FALSE) {
    int length;
    glGetProgramiv(m_Id, GL_INFO_LOG_LENGTH, &length);

    auto info_log = std::vector<char>(static_cast<usize>(length));
    glGetProgramInfoLog(m_Id, length, &length, &info_log[0]);

    glDeleteProgram(m_Id);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    eprint("%s\n", info_log.data());
  }

  glDetachShader(m_Id, vert_shader);
  glDetachShader(m_Id, frag_shader);
}
