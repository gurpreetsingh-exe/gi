#include <compositor.hh>

auto Effect::render() -> void { glDrawArrays(GL_TRIANGLES, 0, 3); }

auto Compositor::add_effect(const std::string& path) -> void {
  m_effects.emplace_back(m_rm, path);
}

auto Compositor::resize(u32 width, u32 height) -> void {
  m_rm->get(m_framebuffers[0]).resize(width, height);
  m_rm->get(m_framebuffers[1]).resize(width, height);
}

auto Compositor::composite(const Texture& texture) -> void {
  auto i = 0;
  for (auto& effect : m_effects) {
    auto& offscreen = m_rm->get(m_framebuffers[m_active_framebuffer]);
    m_active_framebuffer = 1 - m_active_framebuffer;
    auto& framebuffer = m_rm->get(m_framebuffers[m_active_framebuffer]);
    effect.upload_resources([&](auto& shader) {
      glActiveTexture(GL_TEXTURE0);
      if (i == 0) {
        texture.bind();
      } else {
        framebuffer.get_color_attachments()[0].bind();
      }
      shader.upload_uniform_int("u_texture", 0);
    });
    offscreen.bind();
    effect.render();
    offscreen.unbind();
    i += 1;
  }
  m_active_framebuffer = 0;
}
