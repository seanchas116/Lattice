#include "VAO.hpp"
#include "VertexBuffer.hpp"
#include <array>
#include <glm/glm.hpp>

namespace Lattice {

VAO::VAO() : VAO(std::make_shared<VertexBuffer>()) {
}

VAO::VAO(const SP<VertexBuffer> &vertexBuffer) : _vertexBuffer(vertexBuffer) {
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

VAO::VAO(const SP<VertexBuffer> &vertexBuffer, const std::vector<VAO::Triangle> &triangles) : VAO(vertexBuffer) {
    setTriangles(triangles);
}

VAO::~VAO() {
    glDeleteVertexArrays(1, &_vertexArray);
    glDeleteBuffers(1, &_indexBuffer);
}

void VAO::draw() {
    glBindVertexArray(_vertexArray);
    glDrawElements(GL_TRIANGLES, int(_triangles.size() * 3), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void VAO::setTriangles(const std::vector<Triangle> &triangles) {
    _triangles = triangles;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(triangles.size() * sizeof(Triangle)), triangles.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}
