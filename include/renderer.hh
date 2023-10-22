#ifndef RENDERER_H
#define RENDERER_H

#include <framebuffer.hh>
#include <mesh.hh>
#include <shader.hh>
#include <utils.hh>
#include <vertex_array.hh>

class Renderer {
  using ShaderRef = std::shared_ptr<Shader>;

public:
  Renderer(u32 width, u32 height) : m_width(width), m_height(height) {
    m_msaa =
        std::make_unique<Framebuffer<GL_TEXTURE_2D_MULTISAMPLE>>(width, height);
    m_framebuffer = std::make_unique<Framebuffer<GL_TEXTURE_2D>>(width, height);
  }

public:
  auto upload_mesh(Mesh& mesh, ShaderRef shader) -> void;
  auto resize(u32 width, u32 height) -> void;
  auto draw() -> void;
  auto framebuffer() -> Framebuffer<GL_TEXTURE_2D>* {
    return m_framebuffer.get();
  }

private:
  u32 m_width, m_height;
  std::vector<std::tuple<VertexArray, ShaderRef>> m_bindings;
  std::unique_ptr<Framebuffer<GL_TEXTURE_2D>> m_framebuffer;
  std::unique_ptr<Framebuffer<GL_TEXTURE_2D_MULTISAMPLE>> m_msaa;
};

#endif // !RENDERER_H
