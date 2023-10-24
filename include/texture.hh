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

template <u32 target>
class Texture {
public:
  Texture() = default;
  Texture(Texture&& other) {
    m_Id = other.m_Id;
    m_width = other.m_width;
    m_height = other.m_height;
    other.m_Id = 0;
  }

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
      case GL_TEXTURE_CUBE_MAP: {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
        for (u32 i = 0; i < 6; ++i) {
          glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, i32(format),
                       i32(width), i32(height), 0, GL_RGBA, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
        break;
      }
      default:
        assert(false && "Not implemented");
    }
    glBindTexture(target, 0);
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
    glBindTexture(target, 0);
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
        fmt::println("{}", channels);
        assert(0);
      }
    }
    stbi_image_free(data);

    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(target);

    glBindTexture(target, 0);
  }

  ~Texture() { glDeleteTextures(1, &m_Id); }

public:
  void bind() { glBindTexture(target, m_Id); }
  void unbind() { glBindTexture(target, 0); }
  [[nodiscard]] u32 get_id() const { return m_Id; }
  [[nodiscard]] u32 get_width() const { return m_width; }
  [[nodiscard]] u32 get_height() const { return m_height; }

  static auto cubemap(const fs::path& path) -> Texture {
    auto shader = Shader::quad("../shaders/panorama_to_cubemap.glsl");
    auto hdri = Texture<GL_TEXTURE_2D>(path);
    fmt::println("\"{}\" loaded\n  width: {}\n  height: {}", path.string(),
                 hdri.get_width(), hdri.get_height());
    glEnableVertexAttribArray(0);

    auto texture = Texture<GL_TEXTURE_CUBE_MAP>(1024, 1024, GL_RGBA32F);
    u32 fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    texture.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X, texture.get_id(), 0);
    texture.unbind();

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      fmt::println("framebuffer status not complete\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shader.bind();
    auto [w, h] = window.get_size();

    for (u32 i = 0; i < 6; ++i) {
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);
      u32 side = i;
      texture.bind();
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_X + side,
                             texture.get_id(), 0);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glViewport(0, 0, i32(texture.get_width()), i32(texture.get_height()));

      hdri.bind();
      shader.upload_uniform_sampler("u_panorama", 0);
      shader.upload_uniform_int("u_currentFace", i32(i));

      glDrawArrays(GL_TRIANGLES, 0, 3);
      glViewport(0, 0, i32(w), i32(h));
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glDeleteFramebuffers(1, &fbo);

    return texture;
  }

private:
  u32 m_Id;
  u32 m_width, m_height;
};

using CubeMap = Texture<GL_TEXTURE_CUBE_MAP>;

#endif // !TEXTURE_H
