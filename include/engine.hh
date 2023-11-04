#include <editor.hh>
#include <renderer.hh>
#include <resource_manager.hh>
#include <scene.hh>

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
  std::unique_ptr<Scene> m_scene;
  std::unique_ptr<Editor> m_editor;
};
