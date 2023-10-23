#ifndef RENDERER_H
#define RENDERER_H

// clang-format off
#include <GL/glew.h>
// clang-format on
#include <camera.hh>
#include <framebuffer.hh>
#include <mesh.hh>
#include <shader.hh>
#include <utils.hh>
#include <vertex_array.hh>
#include <window.hh>

class Renderer {
  using ShaderRef = std::shared_ptr<Shader>;

public:
  Renderer(u32 width, u32 height) : m_width(width), m_height(height) {
    m_camera = std::make_unique<Camera>(width, height, 90.0, 0.01, 100.0);
    m_msaa =
        std::make_unique<Framebuffer<GL_TEXTURE_2D_MULTISAMPLE>>(width, height);
    m_framebuffer = std::make_unique<Framebuffer<GL_TEXTURE_2D>>(width, height);
    m_cubemap = load_env_map("industrial_sunset_puresky_2k.hdr");
    m_cubemap_vao = std::make_unique<VertexArray>(mesh_to_vao(*Mesh::cube()));
    m_cubemap_shader = std::make_unique<Shader>("../shaders/cubemap_vert.glsl",
                                                "../shaders/cubemap_frag.glsl");
  }

public:
  auto upload_mesh(Mesh& mesh, ShaderRef shader) -> void;
  auto resize(u32 width, u32 height) -> void;
  auto update(std::unique_ptr<Window>& window) -> void;
  auto draw() -> void;
  auto framebuffer() -> Framebuffer<GL_TEXTURE_2D>* {
    return m_framebuffer.get();
  }

private:
  auto draw_cubemap() -> void;
  auto load_env_map(const fs::path& path) -> u32;
  auto mesh_to_vao(Mesh& mesh) -> VertexArray;

private:
  u32 m_width, m_height;
  std::vector<std::tuple<VertexArray, ShaderRef>> m_bindings;
  std::unique_ptr<Framebuffer<GL_TEXTURE_2D>> m_framebuffer;
  std::unique_ptr<Framebuffer<GL_TEXTURE_2D_MULTISAMPLE>> m_msaa;
  std::unique_ptr<VertexArray> m_cubemap_vao;
  std::unique_ptr<Shader> m_cubemap_shader;
  std::unique_ptr<Camera> m_camera;
  u32 m_cubemap;
};

#endif // !RENDERER_H
