#ifndef SCENE_H
#define SCENE_H

#include <entt.hh>
#include <event.hh>

class Entity;
class Editor;

class Scene {
public:
  Scene() = default;
  ~Scene() = default;

public:
  auto resize(u32 width, u32 height) -> void;
  auto add_camera() -> Entity;
  auto create() -> Entity;
  auto create(std::string&& name) -> Entity;
  auto update() -> void;

private:
  auto next_id() -> usize { return disambiguator++; }

private:
  entt::registry m_registry;
  usize disambiguator = 0;
  friend class Entity;
  friend class Editor;
};

#endif // !SCENE_H
