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
  template <Bindable T, typename... Args>
  auto create(Args&&... args) -> Resource<T> {
    auto& resource_vector = std::get<std::vector<T>>(m_resources);
    auto index = u16(resource_vector.size());
    resource_vector.push_back(T(std::forward<Args>(args)...));
    return index;
  }

  auto load_texture(Texture::Type type, const fs::path& path)
      -> Resource<Texture> {
    auto& tex_resource_vector = std::get<std::vector<Texture>>(m_resources);
    auto index = u16(tex_resource_vector.size());
    tex_resource_vector.push_back(Texture(type, m_ldr.load_image(path)));
    return index;
  }

  template <Bindable T>
  auto get(Resource<T> res) [[always_inline]] -> T& {
    auto& resource_vector = std::get<std::vector<T>>(m_resources);
    return resource_vector[res()];
  }

  template <Bindable T>
  auto bind(Resource<T> res) [[always_inline]] -> void {
    get(res).bind();
  }

  template <Bindable T>
  auto unbind(Resource<T> res) [[always_inline]] -> void {
    get(res).unbind();
  }

private:
  // clang-format off
  std::tuple<
    std::vector<Shader>,
    std::vector<Texture>,
    std::vector<Framebuffer>
  > m_resources;
  // clang-format on
  ResourceLoader m_ldr;
};

#endif // !RESOURCE_MANAGER_H
