#include "IndexBuffer.hpp"

namespace Lattice {
namespace GL {

IndexBuffer::IndexBuffer() {
    initializeOpenGLFunctions();
    glGenBuffers(1, &_buffer);
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &_buffer);
}

void IndexBuffer::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer);
}

void IndexBuffer::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // namespace GL
} // namespace Lattice
