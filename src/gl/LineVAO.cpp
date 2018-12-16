#include "LineVAO.hpp"
#include "VertexBuffer.hpp"
#include <array>

namespace Lattice {

LineVAO::LineVAO() : LineVAO(std::make_shared<VertexBuffer>()) {
}

LineVAO::LineVAO(const SP<VertexBuffer> &vertexBuffer) : _vertexBuffer(vertexBuffer) {
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

LineVAO::~LineVAO() {
    glDeleteVertexArrays(1, &_vertexArray);
    glDeleteBuffers(1, &_indexBuffer);
}

void LineVAO::draw() {
    glBindVertexArray(_vertexArray);
    glDrawElements(GL_LINES_ADJACENCY, _lineCount * 4, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void LineVAO::setLineStrips(const std::vector<LineStrip>& strips) {
    std::vector<LineAdjacency> adjacencies;
    for (auto& strip : strips) {
        for (size_t i = 0; i < strip.size() - 1; ++i) {
            size_t prev = std::max(i - 1, size_t(0));
            size_t begin = i;
            size_t end = i + 1;
            size_t next = std::min(i + 2, strip.size() - 1);
            adjacencies.push_back({strip[prev], strip[begin], strip[end], strip[next]});
        }
    }
    setLineAdjacencies(adjacencies);
}

void LineVAO::setLineAdjacencies(const std::vector<LineVAO::LineAdjacency> &lines) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(lines.size() * sizeof(LineAdjacency)), lines.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    _lineCount = int(lines.size());
}

}
