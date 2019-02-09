#include "VAO.hpp"
#include "VertexBuffer.hpp"
#include <array>
#include <glm/glm.hpp>

namespace Lattice::GL {

OldVAO::OldVAO() : OldVAO(makeShared<VertexBuffer<Vertex>>()) {
}

OldVAO::OldVAO(const SP<VertexBuffer<Vertex> > &vertexBuffer) : _vertexBuffer(vertexBuffer) {
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

OldVAO::OldVAO(const SP<VertexBuffer<Vertex> > &vertexBuffer, const std::vector<OldVAO::Triangle> &triangles) : OldVAO(vertexBuffer) {
    setTriangles(triangles);
}

OldVAO::~OldVAO() {
    glDeleteVertexArrays(1, &_vertexArray);
    glDeleteBuffers(1, &_indexBuffer);
}

void OldVAO::draw() {
    glBindVertexArray(_vertexArray);
    glDrawElements(GL_TRIANGLES, int(_triangles.size() * 3), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void OldVAO::setTriangles(const std::vector<Triangle> &triangles) {
    _triangles = triangles;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(triangles.size() * sizeof(Triangle)), triangles.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}
