#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include <camera.hh>

struct CameraComponent {
  Camera camera { 600, 400, 90.0f, 0.01f, 100.0f };
  CameraComponent() = default;
  CameraComponent(Camera& _camera) : camera(_camera) {}
  ~CameraComponent() = default;
};

#endif // !CAMERA_COMPONENT_H
