#include <window.hh>

static void key_callback(GLFWwindow* window, int key, int scancode, int action,
                         int mods) {
  (void)scancode;
  (void)mods;
  auto event = reinterpret_cast<Event*>(glfwGetWindowUserPointer(window));
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      event->disable_cursor = !event->disable_cursor;
    }
    event->pressed[key] = true;
  } else if (action == GLFW_RELEASE) {
    event->pressed[key] = false;
  }
}

static void mouse_callback(GLFWwindow* window, double x, double y) {
  auto event = reinterpret_cast<Event*>(glfwGetWindowUserPointer(window));
  event->mouse_pos = glm::vec2(x, y);
}

Window::Window(u32 width, u32 height, const std::string& name)
    : m_width(width), m_height(height), m_name(std::move(name)) {
  if (!glfwInit()) {
    eprint("cannot initialize glfw\n");
  }

  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
  m_window = glfwCreateWindow(i32(m_width), i32(m_height), m_name.c_str(),
                              nullptr, nullptr);
  if (!m_window) {
    eprint("failed to create window\n");
  }
  get_size();

  m_event = new Event();
  glfwMakeContextCurrent(m_window);
  glfwSetKeyCallback(m_window, key_callback);
  glfwSetCursorPosCallback(m_window, mouse_callback);
  glfwSetWindowUserPointer(m_window, m_event);
  /// disable v-sync
  // glfwSwapInterval(0);
}

auto Window::get_size() -> std::tuple<u32, u32> {
  glfwGetWindowSize(m_window, (i32*)&m_width, (i32*)&m_height);
  return { m_width, m_height };
}

Window::~Window() {
  delete m_event;
  glfwDestroyWindow(m_window);
  glfwTerminate();
}
