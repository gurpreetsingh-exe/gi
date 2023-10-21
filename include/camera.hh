#ifndef CAMERA_H
#define CAMERA_H

#include <event.hh>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  Camera(f32 width, f32 height, f32 fov, f32 clipNear, f32 clipFar)
      : m_ViewportWidth(width), m_ViewportHeight(height), m_Fov(fov),
        m_ClipNear(clipNear), m_ClipFar(clipFar) {
    updateModel();
    updateProjection();
    updateView();
  }

  ~Camera() = default;

public:
  const glm::mat4& getModel() const { return m_Model; }
  const glm::mat4& getView() const { return m_View; }
  const glm::mat4& getProjection() const { return m_Projection; }
  auto update(GLFWwindow* handle, Event* event) -> void;
  auto resize(f32 width, f32 height) -> void;
  auto set_fov(f32 fov) -> void {
    if (m_Fov != fov) {
      m_Fov = fov;
      m_NeedsUpdate = true;
    }
  }

private:
  void updateModel();
  void updateView();
  void updateProjection();

private:
  f32 m_ViewportWidth, m_ViewportHeight;
  glm::mat4 m_Model;
  glm::mat4 m_View;
  glm::mat4 m_Projection;

  f32 m_Fov;
  f32 m_ClipNear;
  f32 m_ClipFar;

  bool m_NeedsUpdate = false;
  bool m_LookAround = false;

  glm::vec3 m_Position = glm::vec3(2.0f, 2.0f, 2.0f);
  glm::vec3 m_Direction = glm::vec3(-1.0f);
  glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);

  glm::vec2 m_LastMousePos = glm::vec2(0.0f);
};

#endif // !CAMERA_H
