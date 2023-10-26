#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
// clang-format off
#include <GL/glew.h>
// clang-format on
#include <texture.hh>

class [[nodiscard]] Framebuffer {
public:
  struct Desc {
    i32 width, height;
    bool multisampled;
  };

  Framebuffer(Desc desc);
  ~Framebuffer();

public:
  auto bind() -> void { glBindFramebuffer(GL_FRAMEBUFFER, m_Id); }
  auto unbind() -> void { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
  auto get_id() -> u32 { return m_Id; }
  auto add_color_attachment() -> void;
  void set_depth_attachment();
  auto resize(u32 width, u32 height) -> void;
  auto invalidate() -> void;

  auto is_complete() -> bool {
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
  }

  [[nodiscard]] auto get_color_attachments() -> std::vector<Texture>& {
    return m_color_attachments;
  }

private:
  u32 m_width, m_height;
  Texture::Type target;
  u32 m_Id;
  std::vector<Texture> m_color_attachments;
  u32 m_depth_attachment;
};

#endif // !FRAMEBUFFER_H
