#include "VertexBuffer.hpp"

namespace Lattice::GL {

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

void AnyVertexBuffer::setVertexData(const void *data, size_t size) {
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(size), data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

} // namespace Lattice
