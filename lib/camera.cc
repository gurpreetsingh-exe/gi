#include <camera.hh>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <window.hh>

#define SPEED 0.005f
#define ROT_SPEED 0.8f

auto Camera::update(GLFWwindow* handle, Event* event) -> void {
  glm::vec2 delta = (event->mouse_pos - m_last_mouse_pos) * 0.0025f;

  glm::vec3 up_dir(0.0, 0.0, 1.0);
  m_right = glm::cross(up_dir, m_direction);

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

  if (event->disable_cursor) {
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_look_around = true;
  } else {
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    m_look_around = false;
  }

  if ((delta.x != 0.0 || delta.y != 0.0) && m_look_around) {
    float pitch = delta.y * ROT_SPEED;
    float yaw = delta.x * ROT_SPEED;
    glm::quat q = glm::normalize(glm::cross(glm::angleAxis(pitch, m_right),
                                            glm::angleAxis(-yaw, up_dir)));
    m_direction = glm::rotate(q, m_direction);
    m_needs_update = true;
  }

  m_last_mouse_pos = event->mouse_pos;

  if (m_needs_update) {
    update_projection();
    update_view();

    m_needs_update = false;
    return;
  }
}
