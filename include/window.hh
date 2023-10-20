#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <utils.hh>

class Window {
public:
  Window(u32 width, u32 height, const std::string& name);
  ~Window();

public:
  auto is_running(std::function<void()>) -> void;
  auto get_handle() -> GLFWwindow* { return m_Window; }
  auto get_width() const -> u32 { return m_Width; }
  auto get_height() const -> u32 { return m_Height; }

private:
  u32 m_Width, m_Height;
  std::string m_Name;
  GLFWwindow* m_Window;
};

#endif // !WINDOW_H
