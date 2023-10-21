#ifndef EVENT_H
#define EVENT_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <utils.hh>

struct Event {
  f32 deltaTime;
  bool pressed[GLFW_KEY_LAST + 1];
  glm::vec2 mousePos;
  bool disableCursor;
};

#endif // !EVENT_H
