#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <framebuffer.hh>
#include <shader.hh>
#include <texture.hh>
#include <utils.hh>

template <typename T>
concept Bindable = requires(T t) {
  { t.bind() };
  { t.unbind() };
};

template <Bindable T>
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

  template <typename... Args>
  auto create_framebuffer(Args&&... args) -> Resource<Framebuffer> {
    auto index = u16(m_framebuffers.size());
    m_framebuffers.push_back(Framebuffer(std::forward<Args>(args)...));
    return index;
  }

  template <Bindable T>
  auto get(Resource<T> res) [[always_inline]] -> T&;

  template <Bindable T>
  auto bind(Resource<T> res) [[always_inline]] -> void {
    get(res).bind();
  }

  template <Bindable T>
  auto unbind(Resource<T> res) [[always_inline]] -> void {
    get(res).unbind();
  }

private:
  std::vector<Shader> m_shaders;
  std::vector<Texture> m_textures;
  std::vector<Framebuffer> m_framebuffers;
  ResourceLoader m_ldr;
};

#endif // !RESOURCE_MANAGER_H
