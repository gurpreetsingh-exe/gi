#ifndef RENDERER_H
#define RENDERER_H

// clang-format off
#include <GL/glew.h>
// clang-format on
#include <camera.hh>
#include <framebuffer.hh>
#include <imgui_layer.hh>
#include <mesh.hh>
#include <shader.hh>
#include <utils.hh>
#include <vertex_array.hh>
#include <window.hh>

extern Window window;

class Renderer {
public:
  Renderer();

public:
  auto upload_mesh(std::unique_ptr<Mesh>&& mesh, Shader&& shader) -> void;
  auto resize(u32 width, u32 height) -> void;
  auto update() -> void;
  auto draw() -> void;
  auto framebuffer() -> Framebuffer<GL_TEXTURE_2D>* { return m_final_fb.get(); }

private:
  auto draw_cubemap() -> void;
  auto mesh_to_vao(std::unique_ptr<Mesh>&& mesh) -> VertexArray;

private:
  u32 m_width, m_height;
  std::vector<std::tuple<VertexArray, Shader>> m_bindings;
  std::unique_ptr<Framebuffer<GL_TEXTURE_2D>> m_framebuffer;
  std::unique_ptr<Framebuffer<GL_TEXTURE_2D_MULTISAMPLE>> m_msaa;
  std::unique_ptr<Framebuffer<GL_TEXTURE_2D>> m_final_fb;
  std::unique_ptr<VertexArray> m_cubemap_vao;
  std::unique_ptr<Shader> m_cubemap_shader;
  std::unique_ptr<Shader> m_post_process_shader;
  std::unique_ptr<Camera> m_camera;
  std::unique_ptr<ImGuiLayer> m_imgui_layer;
  std::unique_ptr<CubeMap> m_cubemap;
};

#endif // !RENDERER_H
