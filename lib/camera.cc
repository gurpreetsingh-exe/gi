#include <camera.hh>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <window.hh>

#define SPEED 0.005f
#define ROT_SPEED 0.8f

extern Window window;

auto Camera::update() -> void {
  auto event = window.get_event();
  auto delta = (event->mouse_pos - m_last_mouse_pos) * 0.0025f;
  m_right = glm::cross(up, m_direction);
  if (event->pressed['W']) {
    m_position += m_direction * event->delta_time * SPEED;
    m_needs_update = true;
  }
  if (event->pressed['S']) {
    m_position -= m_direction * event->delta_time * SPEED;
    m_needs_update = true;
  }
  if (event->pressed['A']) {
    m_position += m_right * event->delta_time * SPEED;
    m_needs_update = true;
  }
  if (event->pressed['D']) {
    m_position -= m_right * event->delta_time * SPEED;
    m_needs_update = true;
  }

  auto handle = window.get_handle();
  if (event->disable_cursor) {
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_look_around = true;
  } else {
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    m_look_around = false;
  }

  if (delta != glm::vec2(0.0) && m_look_around) {
    float pitch = delta.y * ROT_SPEED;
    float yaw = delta.x * ROT_SPEED;
    glm::quat q = glm::normalize(
        glm::cross(glm::angleAxis(pitch, m_right), glm::angleAxis(-yaw, up)));
    m_direction = glm::rotate(q, m_direction);
    m_needs_update = true;
  }

  m_last_mouse_pos = event->mouse_pos;

  if (m_needs_update) {
    update_projection();
    update_view();

    m_needs_update = false;
  }
}
