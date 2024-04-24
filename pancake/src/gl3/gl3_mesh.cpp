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

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(sizeof(Vec4f)));
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(2 * sizeof(Vec4f)));
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(3 * sizeof(Vec4f)));

  glVertexAttribDivisor(0, 0);
  glVertexAttribDivisor(1, 0);
  glVertexAttribDivisor(2, 0);
  glVertexAttribDivisor(3, 0);

  glBindBuffer(GL_ARRAY_BUFFER, _instance_vbo);

  glEnableVertexAttribArray(4);
  glEnableVertexAttribArray(5);
  glEnableVertexAttribArray(6);
  glEnableVertexAttribArray(7);
  glEnableVertexAttribArray(8);
  glEnableVertexAttribArray(9);
  glEnableVertexAttribArray(10);
  glEnableVertexAttribArray(11);
  glEnableVertexAttribArray(12);

  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData), nullptr);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData),
                        reinterpret_cast<void*>(sizeof(Vec4f)));
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData),
                        reinterpret_cast<void*>(2 * sizeof(Vec4f)));
  glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData),
                        reinterpret_cast<void*>(3 * sizeof(Vec4f)));
  glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData),
                        reinterpret_cast<void*>(4 * sizeof(Vec4f)));
  glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData),
                        reinterpret_cast<void*>(5 * sizeof(Vec4f)));
  glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData),
                        reinterpret_cast<void*>(6 * sizeof(Vec4f)));
  glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(CommonPerInstanceData),
                        reinterpret_cast<void*>(7 * sizeof(Vec4f)));
  glVertexAttribIPointer(12, 4, GL_UNSIGNED_INT, sizeof(CommonPerInstanceData),
                         reinterpret_cast<void*>(8 * sizeof(Vec4f)));

  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);
  glVertexAttribDivisor(6, 1);
  glVertexAttribDivisor(7, 1);
  glVertexAttribDivisor(8, 1);
  glVertexAttribDivisor(9, 1);
  glVertexAttribDivisor(10, 1);
  glVertexAttribDivisor(11, 1);
  glVertexAttribDivisor(12, 1);

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