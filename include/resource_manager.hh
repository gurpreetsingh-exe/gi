#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <shader.hh>
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

  auto get_shader(Resource<Shader> shader) -> Shader&;

private:
  std::vector<Shader> m_shaders;
};

#endif // !RESOURCE_MANAGER_H
