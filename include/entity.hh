#ifndef ENTITY_H
#define ENTITY_H

#include <debug.hh>
#include <entt.hh>
#include <scene.hh>
#include <utils.hh>

class Entity {
public:
  Entity() = default;
  Entity(entt::entity handle, Scene* scene)
      : m_handle(handle), m_scene(scene) {}
  Entity(const Entity&) = default;
  ~Entity() = default;

public:
  auto operator==(Entity other) -> bool { return m_handle == other.m_handle; }
  explicit operator bool() {
    return m_handle != entt::null and m_scene != nullptr;
  }

public:
  template <typename T>
  auto has_component() -> bool {
    ASSERT(*this, "default entity");
    return m_scene->m_registry.all_of<T>(m_handle);
  }

  template <typename T, typename... Args>
  auto add_component(Args&&... args) -> T& {
    ASSERT(not has_component<T>(), "already has component");
    return m_scene->m_registry.emplace<T>(m_handle,
                                          std::forward<Args>(args)...);
  }

  template <typename T>
  auto get_component() -> T& {
    ASSERT(has_component<T>(), "component not found");
    return m_scene->m_registry.get<T>(m_handle);
  }

private:
  entt::entity m_handle = entt::null;
  Scene* m_scene = nullptr;
};

#endif // !ENTITY_H
