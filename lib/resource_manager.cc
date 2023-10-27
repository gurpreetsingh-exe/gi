#include <resource_manager.hh>

auto ResourceLoader::load_image(const fs::path& path) -> Texture::Desc {
  i32 w = 0;
  i32 h = 0;
  i32 channels = 0;
  auto data = stbi_loadf(path.c_str(), &w, &h, &channels, 0);
  fmt::println("\"{}\" loaded\n  width: {}\n  height: {}\n  channels: {}",
               path.string(), w, h, channels);
  std::optional<Texture::Format> internal_format = {};
  switch (channels) {
    case 3: {
      internal_format = Texture::Format::RGB32F;
    } break;
    case 4: {
      internal_format = Texture::Format::RGBA32F;
    } break;
    default: {
      fmt::println("{}", channels);
      assert(0);
    }
  }

  if (not internal_format.has_value()) {
    assert(false);
  }

  return Texture::Desc {
    .width = u32(w),
    .height = u32(h),
    .format = *internal_format,
    .byte_format = Texture::ByteFormat::Float,
    .pixels = data,
  };
}

template <>
auto ResourceManager::get<Shader>(Resource<Shader> shader) -> Shader& {
  return m_shaders[shader()];
}

template <>
auto ResourceManager::get<Framebuffer>(Resource<Framebuffer> framebuffer)
    -> Framebuffer& {
  return m_framebuffers[framebuffer()];
}

template <>
auto ResourceManager::get<Texture>(Resource<Texture> texture) -> Texture& {
  return m_textures[texture()];
}
