#include <window.hh>

class Engine {
  static constexpr u32 width = 600;
  static constexpr u32 height = 600;

public:
  Engine() : m_Window(std::make_unique<Window>(width, height, "gi")) {}

public:
  auto run() -> void;

private:
  std::unique_ptr<Window> m_Window;
};
