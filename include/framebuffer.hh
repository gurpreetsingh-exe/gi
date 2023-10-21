#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
// clang-format off
#include <GL/glew.h>
// clang-format on
#include <texture.hh>

template <u32 target>
class Framebuffer {
public:
  Framebuffer(u32 width, u32 height) : m_width(width), m_height(height) {
    glCreateFramebuffers(1, &m_Id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
    add_color_attachment();
    set_depth_attachment();
  }

  ~Framebuffer() {
    fmt::println("~Framebuffer()");
    m_color_attachments.clear();
    glDeleteFramebuffers(1, &m_Id);
  }

public:
  void bind() { glBindFramebuffer(GL_FRAMEBUFFER, m_Id); }

  void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

  uint32_t get_id() { return m_Id; }

  void add_color_attachment() {
    auto& texture =
        m_color_attachments.emplace_back(m_width, m_height, GL_RGB8);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        u32(GL_COLOR_ATTACHMENT0 + m_color_attachments.size() - 1), target,
        texture.get_id(), 0);
    texture.unbind();
  }

  void set_depth_attachment() {
    glCreateTextures(target, 1, &m_depth_attachment);
    glBindTexture(target, m_depth_attachment);
    switch (target) {
      case GL_TEXTURE_2D: {
        glTexStorage2D(target, 1, GL_DEPTH24_STENCIL8, i32(m_width),
                       i32(m_height));
        break;
      }
      case GL_TEXTURE_2D_MULTISAMPLE: {
        glTexStorage2DMultisample(target, 4, GL_DEPTH24_STENCIL8, i32(m_width),
                                  i32(m_height), GL_TRUE);
        break;
      }
      default:
        assert(false && "Not implemented");
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, target,
                           m_depth_attachment, 0);
    glBindTexture(target, 0);
  }

  bool is_complete() {
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
  }

  auto get_color_attachments() -> const std::vector<Texture<target>>& {
    return m_color_attachments;
  }

  void resize(u32 width, u32 height) {
    if (width != m_width || height != m_height) {
      m_width = width;
      m_height = height;
      invalidate();
    }
  }

  void invalidate() {
    bind();
    m_color_attachments.clear();
    glDeleteTextures(1, &m_Id);
    add_color_attachment();
    set_depth_attachment();
    unbind();
  }

private:
  u32 m_Id;
  u32 m_width, m_height;
  std::vector<Texture<target>> m_color_attachments;
  u32 m_depth_attachment;
};

#endif // !FRAMEBUFFER_H
