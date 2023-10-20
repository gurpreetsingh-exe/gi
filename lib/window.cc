#include <window.hh>

// static void keyCallback(GLFWwindow* window, int key, int scancode, int
// action,
//                         int mods) {
//   auto event = reinterpret_cast<Event*>(glfwGetWindowUserPointer(window));
//   if (action == GLFW_PRESS) {
//     if (key == GLFW_KEY_ESCAPE) {
//       event->disableCursor = !event->disableCursor;
//     }
//     event->pressed[key] = true;
//   } else if (action == GLFW_RELEASE) {
//     event->pressed[key] = false;
//   }
// }
//
// static void mouseCallback(GLFWwindow* window, double x, double y) {
//   auto event = reinterpret_cast<Event*>(glfwGetWindowUserPointer(window));
//   event->mousePos = glm::vec2(x, y);
// }

Window::Window(u32 width, u32 height, const std::string& name)
    : m_Width(width), m_Height(height), m_Name(std::move(name)) {
  if (!glfwInit()) {
    eprint("cannot initialize glfw\n");
  }

  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
  m_Window =
      glfwCreateWindow(static_cast<i32>(m_Width), static_cast<i32>(m_Height),
                       m_Name.c_str(), nullptr, nullptr);
  if (!m_Window) {
    eprint("failed to create window\n");
  }

  glfwMakeContextCurrent(m_Window);
  // glfwSetKeyCallback(m_Window, keyCallback);
  // glfwSetCursorPosCallback(m_Window, mouseCallback);
  /// disable v-sync
  // glfwSwapInterval(0);
}

Window::~Window() {
  glfwDestroyWindow(m_Window);
  glfwTerminate();
}
