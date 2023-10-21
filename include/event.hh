#ifndef EVENT_H
#define EVENT_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <utils.hh>

struct Event {
  f32 delta_time;
  bool pressed[GLFW_KEY_LAST + 1];
  glm::vec2 mouse_pos;
  bool disable_cursor;
};

#endif // !EVENT_H
