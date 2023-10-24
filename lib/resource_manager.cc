#include <resource_manager.hh>

auto ResourceManager::get_shader(Resource<Shader> shader) -> Shader& {
  return m_shaders[shader()];
}
