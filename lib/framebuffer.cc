#include <framebuffer.hh>

static u32 g_attachments[4] = {
  GL_COLOR_ATTACHMENT0,
  GL_COLOR_ATTACHMENT1,
  GL_COLOR_ATTACHMENT2,
  GL_COLOR_ATTACHMENT3,
};

Framebuffer::Framebuffer(Desc desc)
    : m_width(desc.width), m_height(desc.height),
      m_att_desc(std::move(desc.att_desc)),
      m_target(desc.multisampled ? Texture::Type::TextureMultiSampled2D
                                 : Texture::Type::Texture2D) {
  glCreateFramebuffers(1, &m_Id);
  invalidate();
}

Framebuffer::Framebuffer(Framebuffer&& other) {
  m_Id = other.m_Id;
  m_width = other.m_width;
  m_height = other.m_height;
  m_target = other.m_target;
  m_att_desc = std::move(other.m_att_desc);
  m_color_attachments = std::move(other.m_color_attachments);
  m_depth_attachment = other.m_depth_attachment;
  other.m_depth_attachment = 0;
  other.m_Id = 0;
}

Framebuffer::~Framebuffer() {
  m_color_attachments.clear();
  glDeleteFramebuffers(1, &m_Id);
}

auto Framebuffer::add_attachments() -> void {
  auto color_attachments = 0L;
  for (const auto& adesc : m_att_desc.attachments) {
    if (adesc.format != Texture::Format::Depth) {
      Texture::Desc desc = { .width = m_width,
                             .height = m_height,
                             .format = adesc.format,
                             .byte_format = adesc.byte_format,
                             .pixels = nullptr };
      auto& texture = m_color_attachments.emplace_back(m_target, desc);
      texture.bind();
      auto type = Texture::type_to_opengl_target(m_target);
      glFramebufferTexture2D(
          GL_FRAMEBUFFER,
          u32(GL_COLOR_ATTACHMENT0 + m_color_attachments.size() - 1), type,
          texture.get_id(), 0);
      texture.unbind();
      color_attachments += 1;
    } else {
      set_depth_attachment();
    }
  }
  if (not color_attachments) {
    glDrawBuffer(GL_NONE);
  } else if (color_attachments > 1) {
    glDrawBuffers(color_attachments, g_attachments);
  }
}

auto Framebuffer::set_depth_attachment() -> void {
  auto type = Texture::type_to_opengl_target(m_target);
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
  dbg("Framebuffer(id = %d)\n", m_Id);
  bind();
  m_color_attachments.clear();
  glDeleteTextures(1, &m_Id);
  add_attachments();
  if (!is_complete()) {
    eprint("framebuffer setup not completed");
  }
  unbind();
}
