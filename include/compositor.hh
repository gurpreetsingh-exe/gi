#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <framebuffer.hh>
#include <resource_manager.hh>
#include <shader.hh>

class Effect {
public:
  Effect(std::shared_ptr<ResourceManager> rm, const std::string& path)
      : m_shader(rm->create<Shader>(Shader::quad(path))), m_rm(rm.get()) {}

public:
  auto render() -> void;

  template <typename Callable>
  auto upload_resources(Callable cb) -> void {
    auto& shader = m_rm->get(m_shader);
    shader.bind();
    cb(shader);
  }

private:
  Resource<Shader> m_shader;
  ResourceManager* m_rm;
};

class Compositor {
public:
  Compositor(std::shared_ptr<ResourceManager> rm) : m_rm(rm) {
    auto desc = Framebuffer::Desc(
        512, 512, { { Texture::Format::RGB8, Texture::ByteFormat::UByte } });
    m_framebuffers = { m_rm->create<Framebuffer>(desc),
                       m_rm->create<Framebuffer>(desc) };
  }

public:
  auto add_effect(const std::string& path) -> void;
  auto composite(const Texture& texture) -> void;
  auto resize(u32 width, u32 height) -> void;
  auto framebuffer() -> Framebuffer& { return m_rm->get(m_framebuffers[0]); }

private:
  std::shared_ptr<ResourceManager> m_rm;
  std::vector<Effect> m_effects;
  std::array<Resource<Framebuffer>, 2> m_framebuffers;
  usize m_active_framebuffer = 0;
};

#endif // !COMPOSITOR_H
