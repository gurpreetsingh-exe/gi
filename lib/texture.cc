#include <loader.hh>
#include <texture.hh>

auto Texture::Desc::opengl_format() -> i32 {
  switch (format) {
    case Format::RGB8:
      return GL_RGB8;
    case Format::RGB16F:
      return GL_RGB16F;
    case Format::RGB32F:
      return GL_RGB32F;
    case Format::RGBA32F:
      return GL_RGBA32F;
    case Format::Depth:
      return GL_DEPTH24_STENCIL8;
    default:
      __builtin_unreachable();
      assert(false && "format not found");
  }
}

auto Texture::Desc::opengl_byte_format() -> i32 {
  switch (byte_format) {
    case ByteFormat::Float:
      return GL_FLOAT;
    case ByteFormat::Int:
      return GL_INT;
    case ByteFormat::UByte:
      return GL_UNSIGNED_BYTE;
    default:
      __builtin_unreachable();
      assert(false && "byte format not found");
  }
}

static auto fmt_from_format(Texture::Format format) -> u32 {
  switch (format) {
    case Texture::Format::RGB8:
    case Texture::Format::RGB32F:
      return GL_RGB;
    case Texture::Format::RGBA32F:
      return GL_RGBA;
    default:
      __builtin_unreachable();
      assert(false && "format not found");
  }
}

Texture::Texture(Type type, Desc desc)
    : m_width(desc.width), m_height(desc.height),
      target(type_to_opengl_target(type)) {
  glCreateTextures(target, 1, &m_Id);
  dbg("Texture(id = %d, target = %d)\n", m_Id, target);
  glBindTexture(target, m_Id);
  create_opengl_texture(desc);
  glBindTexture(target, 0);
}

Texture::Texture(Type type, const fs::path& path)
    : Texture(type, ResourceLoader::load_image(path)) {}

Texture::Texture(Texture&& other) {
  m_Id = other.m_Id;
  m_width = other.m_width;
  m_height = other.m_height;
  target = other.target;
  other.m_Id = 0;
  dbg("moving Texture(id = %d, target = %d)\n", m_Id, target);
}

auto Texture::create_opengl_texture(Desc desc) -> void {
  auto fmt = fmt_from_format(desc.format);
  switch (target) {
    case GL_TEXTURE_2D: {
      // assert(desc.format == Format::RGB32F);
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(target, 0, desc.opengl_format(), desc.width, desc.height, 0,
                   fmt, desc.opengl_byte_format(), desc.pixels);
    } break;
    case GL_TEXTURE_2D_MULTISAMPLE: {
      glTexImage2DMultisample(target, 4, desc.opengl_format(), desc.width,
                              desc.height, GL_TRUE);
    } break;
    case GL_TEXTURE_CUBE_MAP: {
      m_width = 1000;
      m_height = 1000;
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      for (u32 i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                     desc.opengl_format(), m_width, m_height, 0, fmt,
                     desc.opengl_byte_format(), nullptr);
      }
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

      if (not desc.pixels) {
        fmt::println("pixels not found");
        return;
      }

      unbind();
      auto shader = Shader::quad("../shaders/panorama_to_cubemap.glsl");
      auto hdri = Texture(Type::Texture2D, desc);
      desc.pixels = nullptr;
      glEnableVertexAttribArray(0);

      u32 fbo;
      glGenFramebuffers(1, &fbo);
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);
      bind();
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_Id, 0);
      unbind();

      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fmt::println("framebuffer status not complete\n");
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      shader.bind();
      auto [w, h] = window.get_size();

      for (u32 i = 0; i < 6; ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        u32 side = i;
        bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, m_Id, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, i32(m_width), i32(m_height));

        hdri.bind();
        shader.upload_uniform_sampler("u_panorama", 0);
        shader.upload_uniform_int("u_currentFace", i32(i));

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glViewport(0, 0, i32(w), i32(h));
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }
      glDeleteFramebuffers(1, &fbo);
    } break;
    default:
      assert(false && "Not implemented");
  }
  stbi_image_free(desc.pixels);
}
