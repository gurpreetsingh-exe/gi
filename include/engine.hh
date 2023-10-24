#include <renderer.hh>
#include <resource_manager.hh>

class Engine {
  std::string vert = "../shaders/vertex.glsl";
  std::string frag = "../shaders/fragment.glsl";

public:
  Engine();

public:
  auto run() -> void;

private:
  std::unique_ptr<Renderer> m_renderer;
  std::shared_ptr<ResourceManager> m_resource_manager;
};
