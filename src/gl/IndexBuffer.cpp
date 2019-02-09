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

void IndexBuffer::setTriangles(const std::vector<IndexBuffer::Triangle> &triangles) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(triangles.size() * sizeof(Triangle)), triangles.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    _size = triangles.size() * 3;
    _primitive = Primitive::Triangle;
}

void IndexBuffer::setLines(const std::vector<IndexBuffer::Line> &lines) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(lines.size() * sizeof(Line)), lines.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    _size = lines.size() * 2;
    _primitive = Primitive::Line;
}

void IndexBuffer::setLineStrips(const std::vector<IndexBuffer::LineStrip> &strips) {
    std::vector<Line> lines;
    for (auto& strip : strips) {
        for (size_t i = 0; i < strip.size() - 1; ++i) {
            lines.push_back({strip[i], strip[i+1]});
        }
    }
    setLines(lines);
}

} // namespace GL
} // namespace Lattice
