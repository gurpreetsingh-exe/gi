#ifndef RENDERER_H
#define RENDERER_H

// clang-format off
#include <GL/glew.h>
// clang-format on
#include <camera.hh>
#include <entity.hh>
#include <framebuffer.hh>
#include <mesh.hh>
#include <resource_manager.hh>
#include <shader.hh>
#include <utils.hh>
#include <vertex_array.hh>
#include <window.hh>

extern Window window;

class Renderer {
public:
  Renderer(std::shared_ptr<ResourceManager>);

public:
  auto upload_mesh(std::unique_ptr<Mesh>&& mesh, Resource<Shader> shader)
      -> void;
  auto resize(u32 width, u32 height) -> void;
  auto draw() -> void;
  auto framebuffer() -> Framebuffer& {
    return m_resource_manager->get(m_final_fb);
  }
  auto set_active_camera(Entity entity) { m_camera = entity; }

private:
  auto post_process() -> void;
  auto draw_cubemap() -> void;
  auto mesh_to_vao(std::unique_ptr<Mesh>&& mesh) -> VertexArray;

private:
  u32 m_width, m_height;
  std::shared_ptr<ResourceManager> m_resource_manager;
  std::vector<std::tuple<VertexArray, Resource<Shader>>> m_bindings;
  Resource<Framebuffer> m_framebuffer;
  Resource<Framebuffer> m_final_fb;
  std::unique_ptr<VertexArray> m_cubemap_vao;
  Resource<Shader> m_cubemap_shader;
  Resource<Shader> m_post_process_shader;
  Resource<Texture> m_cubemap;
  Entity m_camera;

  glm::mat4 m_prev_view_projection;
};

#endif // !RENDERER_H
