#include "VertexBuffer.hpp"

namespace Lattice::GL {

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoord)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glVertexAttribPointer(3, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, flags)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
}

void VertexBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

} // namespace Lattice
