#include "LineVAO.hpp"
#include "VertexBuffer.hpp"
#include <array>

namespace Lattice::GL {

LineVAO::LineVAO() : LineVAO(makeShared<OldVertexBuffer>()) {
}

LineVAO::LineVAO(const SP<OldVertexBuffer> &vertexBuffer) : _vertexBuffer(vertexBuffer) {
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
    glDrawElements(GL_LINES, _lineCount * 2, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void LineVAO::setLineStrips(const std::vector<LineStrip>& strips) {
    std::vector<Line> lines;
    for (auto& strip : strips) {
        for (size_t i = 0; i < strip.size() - 1; ++i) {
            lines.push_back({strip[i], strip[i+1]});
        }
    }
    setLines(lines);
}

void LineVAO::setLines(const std::vector<LineVAO::Line> &lines) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(lines.size() * sizeof(Line)), lines.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    _lineCount = int(lines.size());
}

}
