#include "VertexBuffer.hpp"

namespace Lattice {
namespace GL {

AnyVertexBuffer::AnyVertexBuffer() {
    initializeOpenGLFunctions();
    glGenBuffers(1, &_buffer);
}

AnyVertexBuffer::~AnyVertexBuffer() {
    glDeleteBuffers(1, &_buffer);
}

void AnyVertexBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
}

void AnyVertexBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AnyVertexBuffer::setVertexData(const void *data, size_t size) {
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(size), data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}
} // namespace Lattice
