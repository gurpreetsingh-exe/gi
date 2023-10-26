#include <framebuffer.hh>

Framebuffer::Framebuffer(Desc desc)
    : m_width(desc.width), m_height(desc.height),
      target(desc.multisampled ? Texture::Type::TextureMultiSampled2D
                               : Texture::Type::Texture2D) {
  glCreateFramebuffers(1, &m_Id);
  glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
  add_color_attachment();
  set_depth_attachment();
  if (!is_complete()) {
    eprint("framebuffer setup not completed");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() {
  m_color_attachments.clear();
  glDeleteFramebuffers(1, &m_Id);
}

auto Framebuffer::add_color_attachment() -> void {
  Texture::Desc desc = { .width = m_width,
                         .height = m_height,
                         .format = Texture::Format::RGB8,
                         .byte_format = Texture::ByteFormat::UByte,
                         .pixels = nullptr };
  auto& texture = m_color_attachments.emplace_back(target, desc);
  auto type = Texture::type_to_opengl_target(target);
  glFramebufferTexture2D(
      GL_FRAMEBUFFER,
      u32(GL_COLOR_ATTACHMENT0 + m_color_attachments.size() - 1), type,
      texture.get_id(), 0);
  texture.unbind();
}

auto Framebuffer::set_depth_attachment() -> void {
  auto type = Texture::type_to_opengl_target(target);
  glCreateTextures(type, 1, &m_depth_attachment);
  glBindTexture(type, m_depth_attachment);
  switch (type) {
    case GL_TEXTURE_2D: {
      glTexStorage2D(type, 1, GL_DEPTH24_STENCIL8, i32(m_width), i32(m_height));
      break;
    }
    case GL_TEXTURE_2D_MULTISAMPLE: {
      glTexStorage2DMultisample(type, 4, GL_DEPTH24_STENCIL8, i32(m_width),
                                i32(m_height), GL_TRUE);
      break;
    }
    default:
      assert(false && "Not implemented");
  }
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, type,
                         m_depth_attachment, 0);
  glBindTexture(type, 0);
}

auto Framebuffer::resize(u32 width, u32 height) -> void {
  if (width != m_width || height != m_height) {
    m_width = width;
    m_height = height;
    invalidate();
  }
}

auto Framebuffer::invalidate() -> void {
  bind();
  m_color_attachments.clear();
  glDeleteTextures(1, &m_Id);
  add_color_attachment();
  set_depth_attachment();
  unbind();
}
