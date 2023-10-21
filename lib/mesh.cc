#include <mesh.hh>
#define TINYOBJLOADER_IMPLEMENTATION
#include "../third_party/tiny_obj_loader.h"

auto Mesh::from_obj(const fs::path& filepath) -> std::unique_ptr<Mesh> {
  auto mesh = std::make_unique<Mesh>();
  auto reader = tinyobj::ObjReader();
  auto config = tinyobj::ObjReaderConfig();
  reader.ParseFromFile(filepath, config);

  if (!reader.Valid()) {
    eprint("%s", (reader.Warning() + reader.Error()).c_str());
  }

  auto unique_vertices = std::unordered_map<Vertex, u32>();
  const auto& attributes = reader.GetAttrib();

  for (const auto& shape : reader.GetShapes()) {
    for (const auto& index : shape.mesh.indices) {
      auto vertex = Vertex();
      vertex.pos = {
        attributes.vertices[static_cast<usize>(3 * index.vertex_index + 0)],
        attributes.vertices[static_cast<usize>(3 * index.vertex_index + 1)],
        attributes.vertices[static_cast<usize>(3 * index.vertex_index + 2)],
      };

      if (unique_vertices.count(vertex) == 0) {
        unique_vertices[vertex] = static_cast<u32>(mesh->vertices.size());
        mesh->vertices.push_back(vertex);
      }

      mesh->indices.push_back(unique_vertices[vertex]);
    }
  }
  fmt::println("\"{}\" loaded\n  vertices: {}\n  indices: {}",
               filepath.string(), mesh->vertices.size(), mesh->indices.size());
  return mesh;
}
