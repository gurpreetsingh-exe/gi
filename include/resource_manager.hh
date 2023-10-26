#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <shader.hh>
#include <texture.hh>
#include <utils.hh>

template <typename T>
struct Resource {
  u32 handle = 0;
  Resource() = default;
  Resource(u16 index) { handle = index; }
  [[gnu::always_inline]] auto operator()() -> usize {
    return usize(handle & 0xffff);
  }
};

struct ResourceLoader {
  ResourceLoader() = default;

  auto load_image(const fs::path& path) -> Texture::Desc;
};

class ResourceManager {
public:
  ResourceManager() = default;
  ResourceManager(ResourceManager&&) = delete;
  ~ResourceManager() = default;

public:
  template <typename... Args>
  auto load_shader(Args&&... args) -> Resource<Shader> {
    auto index = u16(m_shaders.size());
    m_shaders.push_back(Shader(std::forward<Args>(args)...));
    return index;
  }

  auto load_texture(Texture::Type type, const fs::path& path)
      -> Resource<Texture> {
    auto index = u16(m_textures.size());
    m_textures.push_back(Texture(type, m_ldr.load_image(path)));
    return index;
  }

  auto get_shader(Resource<Shader> shader) -> Shader& {
    return m_shaders[shader()];
  }

  auto get_texture(Resource<Texture> texture) -> Texture& {
    return m_textures[texture()];
  }

private:
  std::vector<Shader> m_shaders;
  std::vector<Texture> m_textures;
  ResourceLoader m_ldr;
};

#endif // !RESOURCE_MANAGER_H
