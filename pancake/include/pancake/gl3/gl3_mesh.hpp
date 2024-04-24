#pragma once

#include "graphics/mesh.hpp"

#include <span>

namespace pancake {
class GL3Renderer;
class GL3Mesh : public Mesh {
 public:
  virtual ~GL3Mesh();

  virtual void update(std::span<const Vertex> vertices,
                      std::span<const unsigned int> indices) override;
  virtual void draw(unsigned int num_instances = 1) const override;

 private:
  GL3Mesh(const GUID& guid, unsigned int instance_vbo);
  GL3Mesh(const GUID& guid,
          std::span<const Vertex> vertices,
          std::span<const unsigned int> indices,
          unsigned int instance_vbo);

  friend GL3Renderer;

  unsigned int _vao;
  unsigned int _vbo;
  unsigned int _ebo;
  unsigned int _num_indices;
  unsigned int _instance_vbo;
};
}  // namespace pancake