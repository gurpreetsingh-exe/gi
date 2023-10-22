#ifndef TEXTURE_H
#define TEXTURE_H

// clang-format off
#include <GL/glew.h>
// clang-format on
#include "../third_party/stb/stb_image.h"
#include <utils.hh>

template <u32 target>
class Texture {
public:
  Texture() = default;
  Texture(u32 width, u32 height, u32 format)
      : m_width(width), m_height(height) {
    glCreateTextures(target, 1, &m_Id);
    glBindTexture(target, m_Id);
    switch (target) {
      case GL_TEXTURE_2D: {
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(target, 0, i32(format), i32(width), i32(height), 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        break;
      }
      case GL_TEXTURE_2D_MULTISAMPLE: {
        glTexImage2DMultisample(target, 4, format, i32(width), i32(height),
                                GL_TRUE);
        break;
      }
      default:
        assert(false && "Not implemented");
    }
  }

  Texture(u32 width, u32 height, u32 format, int pixel_type, void* data)
      : m_width(width), m_height(height) {
    glCreateTextures(target, 1, &m_Id);
    glBindTexture(target, m_Id);
    switch (target) {
      case GL_TEXTURE_2D: {
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(target, 0, format, width, height, 0, GL_RGBA, pixel_type,
                     data);
        break;
      }
      case GL_TEXTURE_2D_MULTISAMPLE: {
        glTexImage2DMultisample(target, 4, format, width, height, GL_TRUE);
        break;
      }
      default:
        assert(false && "Not implemented");
    }
  }

  Texture(const std::string& filepath) {
    int w = 0;
    int h = 0;
    int channels = 0;
    auto data = stbi_loadf(filepath.c_str(), &w, &h, &channels, 0);
    m_width = u32(w);
    m_height = u32(h);

    glCreateTextures(target, 1, &m_Id);
    glBindTexture(target, m_Id);

    i32 internal_format = 0x00;
    u32 byte_format;
    switch (channels) {
      case 3: {
        internal_format = GL_RGB32F;
        byte_format = GL_FLOAT;

        glTexImage2D(target, 0, internal_format, w, h, 0, GL_RGB, byte_format,
                     data);
      } break;
      case 4: {
        internal_format = GL_RGBA32F;
        byte_format = GL_FLOAT;

        glTexImage2D(target, 0, internal_format, w, h, 0, GL_RGBA, byte_format,
                     data);
      } break;
      default: {
        assert(0);
      }
    }
    stbi_image_free(data);

    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(target);
  }

  ~Texture() { glDeleteTextures(1, &m_Id); }

public:
  void bind() { glBindTexture(target, m_Id); }
  void unbind() { glBindTexture(target, 0); }
  [[nodiscard]] u32 get_id() const { return m_Id; }
  [[nodiscard]] u32 get_width() const { return m_width; }
  [[nodiscard]] u32 get_height() const { return m_height; }

private:
  u32 m_Id;
  u32 m_width, m_height;
};

#endif // !TEXTURE_H
