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
  Renderer() {
    auto [w, h] = window.get_size();
    m_camera = std::make_unique<Camera>(w, h, 70.0, 0.01, 100.0);
    m_msaa = std::make_unique<Framebuffer<GL_TEXTURE_2D_MULTISAMPLE>>(w, h);
    m_framebuffer = std::make_unique<Framebuffer<GL_TEXTURE_2D>>(w, h);
    m_final_fb = std::make_unique<Framebuffer<GL_TEXTURE_2D>>(w, h);
    m_cubemap = load_env_map("industrial_sunset_puresky_2k.hdr");
    m_cubemap_vao = std::make_unique<VertexArray>(mesh_to_vao(Mesh::cube()));
    m_cubemap_shader = std::make_unique<Shader>("../shaders/cubemap_vert.glsl",
                                                "../shaders/cubemap_frag.glsl");
    m_imgui_layer = std::make_unique<ImGuiLayer>(window.get_handle());
    m_post_process_shader =
        std::make_unique<Shader>(Shader::quad("../shaders/post.glsl"));
  }

public:
  auto upload_mesh(std::unique_ptr<Mesh>&& mesh, Shader&& shader) -> void;
  auto resize(u32 width, u32 height) -> void;
  auto update() -> void;
  auto draw() -> void;
  auto framebuffer() -> Framebuffer<GL_TEXTURE_2D>* { return m_final_fb.get(); }

private:
  auto draw_cubemap() -> void;
  auto load_env_map(const fs::path& path) -> u32;
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
  u32 m_cubemap;
};

#endif // !RENDERER_H
