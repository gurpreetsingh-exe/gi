#ifndef RENDERER_H
#define RENDERER_H

// clang-format off
#include <GL/glew.h>
// clang-format on
#include <camera.hh>
#include <framebuffer.hh>
#include <imgui_layer.hh>
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
  auto update() -> void;
  auto draw() -> void;
  auto framebuffer() -> Framebuffer* { return m_final_fb.get(); }

private:
  auto draw_cubemap() -> void;
  auto mesh_to_vao(std::unique_ptr<Mesh>&& mesh) -> VertexArray;

private:
  u32 m_width, m_height;
  std::shared_ptr<ResourceManager> m_resource_manager;
  std::vector<std::tuple<VertexArray, Resource<Shader>>> m_bindings;
  std::unique_ptr<Framebuffer> m_framebuffer;
  std::unique_ptr<Framebuffer> m_msaa;
  std::unique_ptr<Framebuffer> m_final_fb;
  std::unique_ptr<VertexArray> m_cubemap_vao;
  Resource<Shader> m_cubemap_shader;
  Resource<Shader> m_post_process_shader;
  std::unique_ptr<Camera> m_camera;
  std::unique_ptr<ImGuiLayer> m_imgui_layer;
  Resource<Texture> m_cubemap;
};

#endif // !RENDERER_H
