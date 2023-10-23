#ifndef MESH_H
#define MESH_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <utils.hh>

struct Vertex {
  Vertex() = default;
  Vertex(f32 x, f32 y, f32 z) : pos(glm::vec3(x, y, z)) {}

  glm::vec3 pos;
  auto operator==(const Vertex& other) const -> bool {
    return pos == other.pos;
  }
};

namespace std {
template <>
struct hash<Vertex> {
  auto operator()(Vertex const& vertex) const -> usize {
    return (hash<glm::vec3>()(vertex.pos) >> 1);
  }
};
} // namespace std

struct Mesh {
  enum class Status {
    Undefined,
    Loaded,
  };
  Status status = Status::Undefined;
  std::vector<Vertex> vertices;
  std::vector<u32> indices;

  static auto from_obj(const fs::path& filepath) -> std::unique_ptr<Mesh>;
  static auto cube() -> std::unique_ptr<Mesh>;
};

#endif // !MESH_H
