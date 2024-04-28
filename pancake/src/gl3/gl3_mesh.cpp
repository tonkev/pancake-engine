#include "gl3/gl3_mesh.hpp"

#include "gl3/gl3_renderer.hpp"

#include "GL/gl3w.h"

using namespace pancake;

GL3Mesh::GL3Mesh(const GUID& guid, unsigned int instance_vbo)
    : Mesh(guid), _vao(0), _vbo(0), _ebo(0), _num_indices(0), _instance_vbo(instance_vbo) {}

GL3Mesh::GL3Mesh(const GUID& guid,
                 std::span<const Vertex> vertices,
                 std::span<const unsigned int> indices,
                 unsigned int instance_vbo)
    : Mesh(guid), _vao(0), _vbo(0), _ebo(0), _num_indices(0), _instance_vbo(instance_vbo) {
  update(vertices, indices);
}

GL3Mesh::~GL3Mesh() {
  glDeleteVertexArrays(1, &_vao);
  glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ebo);
}

void GL3Mesh::update(std::span<const Vertex> vertices, std::span<const unsigned int> indices) {
  glDeleteVertexArrays(1, &_vao);
  glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ebo);

  glGenVertexArrays(1, &_vao);
  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ebo);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(),
               GL_STATIC_DRAW);

  glBindVertexArray(_vao);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);
  glEnableVertexAttribArray(5);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(sizeof(Vec4f)));
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(2 * sizeof(Vec4f)));
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(3 * sizeof(Vec4f)));
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(4 * sizeof(Vec4f)));
  glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>((4 * sizeof(Vec4f)) + sizeof(Vec2f)));

  glVertexAttribDivisor(0, 0);
  glVertexAttribDivisor(1, 0);
  glVertexAttribDivisor(2, 0);
  glVertexAttribDivisor(3, 0);
  glVertexAttribDivisor(4, 0);
  glVertexAttribDivisor(5, 0);

  glBindBuffer(GL_ARRAY_BUFFER, _instance_vbo);

  unsigned int iis = 6;

  glEnableVertexAttribArray(iis);
  glEnableVertexAttribArray(iis + 1);
  glEnableVertexAttribArray(iis + 2);
  glEnableVertexAttribArray(iis + 3);
  glEnableVertexAttribArray(iis + 4);
  glEnableVertexAttribArray(iis + 5);
  glEnableVertexAttribArray(iis + 6);
  glEnableVertexAttribArray(iis + 7);
  glEnableVertexAttribArray(iis + 8);

  glVertexAttribPointer(iis, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData), nullptr);
  glVertexAttribPointer(iis + 1, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData),
                        reinterpret_cast<void*>(sizeof(Vec4f)));
  glVertexAttribPointer(iis + 2, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData),
                        reinterpret_cast<void*>(2 * sizeof(Vec4f)));
  glVertexAttribPointer(iis + 3, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData),
                        reinterpret_cast<void*>(3 * sizeof(Vec4f)));
  glVertexAttribPointer(iis + 4, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData),
                        reinterpret_cast<void*>(4 * sizeof(Vec4f)));
  glVertexAttribPointer(iis + 5, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData),
                        reinterpret_cast<void*>(5 * sizeof(Vec4f)));
  glVertexAttribPointer(iis + 6, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData),
                        reinterpret_cast<void*>(6 * sizeof(Vec4f)));
  glVertexAttribPointer(iis + 7, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData),
                        reinterpret_cast<void*>(7 * sizeof(Vec4f)));
  glVertexAttribIPointer(iis + 8, 4, GL_UNSIGNED_INT, sizeof(CommonPerInstanceData),
                         reinterpret_cast<void*>(8 * sizeof(Vec4f)));

  glVertexAttribDivisor(iis, 1);
  glVertexAttribDivisor(iis + 1, 1);
  glVertexAttribDivisor(iis + 2, 1);
  glVertexAttribDivisor(iis + 3, 1);
  glVertexAttribDivisor(iis + 4, 1);
  glVertexAttribDivisor(iis + 5, 1);
  glVertexAttribDivisor(iis + 6, 1);
  glVertexAttribDivisor(iis + 7, 1);
  glVertexAttribDivisor(iis + 8, 1);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  _num_indices = static_cast<unsigned int>(indices.size());
}

void GL3Mesh::draw(unsigned int num_instances) const {
  glBindVertexArray(_vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(_num_indices), GL_UNSIGNED_INT,
                          nullptr, num_instances);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}