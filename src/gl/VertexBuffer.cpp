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

AnyVertexBuffer::AnyVertexBuffer() {
    initializeOpenGLFunctions();
    glGenBuffers(1, &_buffer);
}

AnyVertexBuffer::~AnyVertexBuffer() {
    glDeleteBuffers(1, &_buffer);
}

void AnyVertexBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);

    auto attributes = this->attributes();
    int stride = 0;
    for (auto& a : attributes) {
        stride += a.sizePerComponent * a.count;
    }
    int offset = 0;

    for (size_t i = 0; i < attributes.size(); ++i) {
        auto& attribute = attributes[i];
        glVertexAttribPointer(GLuint(i), attribute.count, attribute.type, GL_FALSE, stride, reinterpret_cast<void*>(offset));
        offset += attribute.sizePerComponent * attribute.count;
        glEnableVertexAttribArray(GLuint(i));
    }
}

void AnyVertexBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AnyVertexBuffer::setVertices(void *data, size_t size) {
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(size), data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

} // namespace Lattice
