#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <event.hh>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  Camera(f32 width, f32 height, f32 fov, f32 clip_near, f32 clip_far)
      : m_viewport_width(width), m_viewport_height(height), m_fov(fov),
        m_clip_near(clip_near), m_clip_far(clip_far) {
    update_model();
    update_projection();
    update_view();
  }

  ~Camera() = default;

public:
  auto get_model() const -> const glm::mat4& { return m_model; }
  auto get_view() const -> const glm::mat4& { return m_view; }
  auto get_projection() const -> const glm::mat4& { return m_projection; }
  auto update(GLFWwindow* handle, Event* event) -> void;

  auto resize(f32 width, f32 height) -> void {
    if (m_viewport_width != width || m_viewport_height != height) {
      m_viewport_width = width;
      m_viewport_height = height;
      fmt::println("{} {}", m_viewport_width, m_viewport_height);
      m_needs_update = true;
    }
  }

  auto set_fov(f32 fov) -> void {
    if (m_fov != fov) {
      m_fov = fov;
      m_needs_update = true;
    }
  }

private:
  auto update_model() -> void { m_model = glm::mat4(1.0f); }

  auto update_view() -> void {
    m_view = glm::lookAt(m_position, m_position + m_direction,
                         glm::vec3(0.0f, 0.0f, 1.0f));
  }

  auto update_projection() -> void {
    m_projection = glm::perspective(glm::radians(m_fov),
                                    m_viewport_width / m_viewport_height,
                                    m_clip_near, m_clip_far);
  }

private:
  f32 m_viewport_width, m_viewport_height;
  glm::mat4 m_model;
  glm::mat4 m_view;
  glm::mat4 m_projection;

  f32 m_fov;
  f32 m_clip_near;
  f32 m_clip_far;

  bool m_needs_update = false;
  bool m_look_around = false;

  glm::vec3 m_position = glm::vec3(2.0f, 2.0f, 2.0f);
  glm::vec3 m_direction = glm::vec3(-1.0f);
  glm::vec3 m_right = glm::vec3(1.0f, 0.0f, 0.0f);

  glm::vec2 m_last_mouse_pos = glm::vec2(0.0f);
};

#endif // !CAMERA_H
