#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
// clang-format off
#include <GL/glew.h>
// clang-format on
#include <texture.hh>

class [[nodiscard]] Framebuffer {
public:
  struct Attachment {
    Texture::Format format;
    Texture::ByteFormat byte_format = Texture::ByteFormat::UByte;
  };

  struct TextureAttachmentDesc {
    std::vector<Attachment> attachments;
    TextureAttachmentDesc() = default;
    TextureAttachmentDesc(std::initializer_list<Attachment> att)
        : attachments(att) {}
  };

  struct Desc {
    i32 width, height;
    TextureAttachmentDesc att_desc;
    bool multisampled = false;

    Desc(i32 w, i32 h, TextureAttachmentDesc att)
        : width(w), height(h), att_desc(std::move(att)) {}
  };

  Framebuffer(Desc desc);
  Framebuffer(Framebuffer&&);
  ~Framebuffer();

public:
  auto bind() -> void { glBindFramebuffer(GL_FRAMEBUFFER, m_Id); }
  auto unbind() -> void { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
  auto get_id() -> u32 { return m_Id; }
  auto depth_attachment() const -> u32 { return m_depth_attachment; }
  auto add_attachments() -> void;
  auto set_depth_attachment() -> void;
  auto resize(u32 width, u32 height) -> void;
  auto invalidate() -> void;

  auto is_complete() -> bool {
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
  }

  [[nodiscard]] auto get_color_attachments() const
      -> const std::vector<Texture>& {
    return m_color_attachments;
  }

private:
  u32 m_width, m_height;
  TextureAttachmentDesc m_att_desc;
  Texture::Type m_target;
  u32 m_Id;
  std::vector<Texture> m_color_attachments;
  u32 m_depth_attachment;
};

#endif // !FRAMEBUFFER_H
