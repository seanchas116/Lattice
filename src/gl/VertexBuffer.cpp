#include "VertexBuffer.hpp"

namespace Lattice {

VertexBuffer::VertexBuffer() {
    initializeOpenGLFunctions();
    glGenBuffers(1, &_buffer);
}

VertexBuffer::VertexBuffer(const std::vector<Vertex> &vertices) : VertexBuffer() {
    setVertices(vertices);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &_buffer);
}

void VertexBuffer::setVertices(const std::vector<Vertex> &vertices) {
    _vertices = vertices;
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void VertexBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

} // namespace Lattice
