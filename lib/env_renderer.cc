#include <env_renderer.hh>
#include <framebuffer.hh>
#include <shader.hh>
#include <texture.hh>

auto render_env_map() -> u32 {
  auto shader = Shader("../shaders/fullscreen.glsl",
                       "../shaders/panorama_to_cubemap.glsl");

  auto hdri = Texture<GL_TEXTURE_2D>("reading_room_1k.hdr");
  hdri.bind();

  u32 cubemap;
  glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

  for (u32 i = 0; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, 128, 128, 0,
                 GL_RGBA, GL_FLOAT, nullptr);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  u32 fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_CUBE_MAP_POSITIVE_X, cubemap, 0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    fmt::println("framebuffer status not complete\n");
  }

  // glBindVertexArray(0);
  glEnableVertexAttribArray(0);

  for (u32 i = 0; i < 6; ++i) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    u32 side = i;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, cubemap, 0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

    glViewport(0, 0, 128, 128);
    // glClearColor(0.5f, 0.5f, 0.5f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.bind();
    glActiveTexture(GL_TEXTURE0);
    hdri.bind();

    shader.upload_uniform_int("u_panorama", i32(hdri.get_id()));
    shader.upload_uniform_int("u_currentFace", i32(i));

    glDrawArrays(GL_TRIANGLES, 0, 3);
  }

  return cubemap;
}
