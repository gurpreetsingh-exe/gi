#include <engine.hh>

auto Engine::run() -> void {
  m_Window->is_running([] {});
}
