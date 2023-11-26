#ifndef TEXTURE_H
#define TEXTURE_H

// clang-format off
#include <GL/glew.h>
// clang-format on
#include "../third_party/stb/stb_image.h"
#include <shader.hh>
#include <utils.hh>
#include <window.hh>

extern Window window;

class [[nodiscard]] Texture {
public:
  enum Type : u8 {
    Texture2D,
    Texture3D,
    TextureMultiSampled2D,
    TextureCube,
  };

  static auto type_to_opengl_target(Type type) -> u32 {
    switch (type) {
      case Type::Texture2D:
        return GL_TEXTURE_2D;
      case Type::Texture3D:
        return GL_TEXTURE_3D;
      case Type::TextureCube:
        return GL_TEXTURE_CUBE_MAP;
      case Type::TextureMultiSampled2D:
        return GL_TEXTURE_2D_MULTISAMPLE;
      default:
        __builtin_unreachable();
        assert(false);
    }
  }

  enum Format : u8 {
    RGB8,
    RGB16F,
    RGB32F,
    RGBA32F,
    Depth,
  };

  enum ByteFormat : u8 {
    Float,
    Int,
    UByte,
  };

  struct Desc {
    u32 width, height;
    Format format;
    ByteFormat byte_format;
    void* pixels;

    auto opengl_format() -> i32;
    auto opengl_byte_format() -> i32;
  };

  Texture() = default;
  Texture(Type type, Desc desc);
  Texture(Type type, const fs::path& path);
  Texture(Texture&& other);
  ~Texture() {
    dbg("~Texture(%d)\n", m_Id);
    glDeleteTextures(1, &m_Id);
  }

public:
  auto bind() const -> void { glBindTexture(target, m_Id); }
  auto unbind() -> void { glBindTexture(target, 0); }
  [[nodiscard]] auto get_id() const -> u32 { return m_Id; }
  [[nodiscard]] auto get_width() const -> u32 { return m_width; }
  [[nodiscard]] auto get_height() const -> u32 { return m_height; }

private:
  auto create_opengl_texture(Desc desc) -> void;

private:
  u32 m_Id;
  u32 m_width, m_height;
  u32 target;
};

#endif // !TEXTURE_H
