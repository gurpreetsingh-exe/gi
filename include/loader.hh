#ifndef LOADER_H
#define LOADER_H

#include <texture.hh>

struct ResourceLoader {
  ResourceLoader() = default;

  static auto load_image(const fs::path& path) -> Texture::Desc;
};

#endif // !LOADER_H
