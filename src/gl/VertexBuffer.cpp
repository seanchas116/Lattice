#include "VertexBuffer.hpp"

namespace Lattice::GL {

OldVertexBuffer::OldVertexBuffer() {
    initializeOpenGLFunctions();
    glGenBuffers(1, &_buffer);
}

OldVertexBuffer::OldVertexBuffer(const std::vector<Vertex> &vertices) : OldVertexBuffer() {
    setVertices(vertices);
}

OldVertexBuffer::~OldVertexBuffer() {
    glDeleteBuffers(1, &_buffer);
}

void OldVertexBuffer::setVertices(const std::vector<Vertex> &vertices) {
    _size = vertices.size();
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OldVertexBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoord)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
}

void OldVertexBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

AnyVertexBuffer::~AnyVertexBuffer() {
}

} // namespace Lattice
