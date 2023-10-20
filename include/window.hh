#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <utils.hh>

class Window {
public:
  Window(u32 width, u32 height, const std::string& name);
  ~Window();

public:
  template <typename Callable>
  auto is_running(Callable cb) -> void {
    while (!glfwWindowShouldClose(m_Window)) {
      cb();
      glfwPollEvents();
      glfwSwapBuffers(m_Window);
    }
  }

  auto get_handle() -> GLFWwindow* { return m_Window; }
  auto get_width() const -> u32 { return m_Width; }
  auto get_height() const -> u32 { return m_Height; }

private:
  u32 m_Width, m_Height;
  std::string m_Name;
  GLFWwindow* m_Window;
};

#endif // !WINDOW_H
