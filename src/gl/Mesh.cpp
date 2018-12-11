#include "Mesh.hpp"
#include "VertexBuffer.hpp"
#include <array>
#include <glm/glm.hpp>

namespace Lattice {

Mesh::Mesh() : Mesh(std::make_shared<VertexBuffer>()) {
}

Mesh::Mesh(const SP<VertexBuffer> &vertexBuffer) : _vertexBuffer(vertexBuffer) {
    initializeOpenGLFunctions();

    glGenBuffers(1, &_indexBuffer);
    glGenVertexArrays(1, &_vertexArray);

    glBindVertexArray(_vertexArray);
    vertexBuffer->bind();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);

    glBindVertexArray(0);
    vertexBuffer->unbind();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Mesh::Mesh(const SP<VertexBuffer> &vertexBuffer, const std::vector<Mesh::Triangle> &triangles) : Mesh(vertexBuffer) {
    setTriangles(triangles);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &_vertexArray);
    glDeleteBuffers(1, &_indexBuffer);
}

void Mesh::draw() {
    glBindVertexArray(_vertexArray);
    glDrawElements(GL_TRIANGLES, int(_triangles.size() * 3), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::setTriangles(const std::vector<Triangle> &triangles) {
    _triangles = triangles;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(triangles.size() * sizeof(Triangle)), triangles.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}
