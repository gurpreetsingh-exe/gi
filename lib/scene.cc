#include <components/camera.hh>
#include <components/identifier.hh>
#include <entity.hh>
#include <scene.hh>

auto Scene::create() -> Entity {
  auto entity = Entity(m_registry.create(), this);
  std::string name = fmt::format("entity{}", next_id());
  entity.add_component<IdentifierComponent>(name);
  return entity;
}

auto Scene::add_camera() -> Entity {
  auto entity = create();
  entity.add_component<CameraComponent>();
  return entity;
}

auto Scene::resize(u32 width, u32 height) -> void {
  auto cameras = m_registry.view<CameraComponent>();
  for (auto& camera : cameras) {
    auto& component = m_registry.get<CameraComponent>(camera);
    component.camera.resize(width, height);
  }
}

auto Scene::update() -> void {
  auto cameras = m_registry.view<CameraComponent>();
  for (auto& camera : cameras) {
    auto& component = m_registry.get<CameraComponent>(camera);
    component.camera.update();
  }
}
