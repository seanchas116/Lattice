#include "LineMesh.hpp"
#include "VertexBuffer.hpp"
#include <array>

namespace Lattice {

LineMesh::LineMesh() : LineMesh(std::make_shared<VertexBuffer>()) {
}

LineMesh::LineMesh(const SP<VertexBuffer> &vertexBuffer) : _vertexBuffer(vertexBuffer) {
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

LineMesh::~LineMesh() {
    glDeleteVertexArrays(1, &_vertexArray);
    glDeleteBuffers(1, &_indexBuffer);
}

void LineMesh::draw() {
    glBindVertexArray(_vertexArray);
    glDrawElements(GL_LINES_ADJACENCY, _lineCount * 4, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void LineMesh::setLineStrips(const std::vector<LineStrip>& strips) {
    std::vector<LineAdjacency> adjacencies;
    for (auto& strip : strips) {
        for (size_t i = 0; i < strip.size() - 1; ++i) {
            size_t prev = std::max(i - i, size_t(0));
            size_t begin = i;
            size_t end = i + 1;
            size_t next = std::min(i + 2, strip.size() - 1);
            adjacencies.push_back({strip[prev], strip[begin], strip[end], strip[next]});
        }
    }
    setLineAdjacencies(adjacencies);
}

void LineMesh::setLineAdjacencies(const std::vector<LineMesh::LineAdjacency> &lines) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(lines.size() * sizeof(LineAdjacency)), lines.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    _lineCount = int(lines.size());
}

}
