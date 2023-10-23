#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <event.hh>
#include <utils.hh>

class Window {
public:
  Window(u32 width, u32 height, const std::string& name);
  ~Window();

public:
  template <typename Callable>
  auto is_running(Callable cb) -> void {
    while (!glfwWindowShouldClose(m_window)) {
      auto time = chr::steady_clock::now();
      cb();
      glfwPollEvents();
      glfwSwapBuffers(m_window);
      m_event->delta_time = f32(
          chr::duration_cast<chr::milliseconds>(time - m_last_time).count());
      m_last_time = time;
    }
  }

  auto get_size() -> std::tuple<u32, u32>;
  auto get_handle() -> GLFWwindow* { return m_window; }
  auto get_width() const -> u32 { return m_width; }
  auto get_height() const -> u32 { return m_height; }
  auto get_event() -> Event* { return m_event; }

private:
  u32 m_width, m_height;
  std::string m_name;
  GLFWwindow* m_window;
  Event* m_event;
  chr::time_point<chr::steady_clock> m_last_time;
};

#endif // !WINDOW_H
