#include "PointsVAO.hpp"
#include "VertexBuffer.hpp"

namespace Lattice {

PointsVAO::PointsVAO() : PointsVAO(std::make_shared<VertexBuffer>()) {
}

PointsVAO::PointsVAO(const SP<VertexBuffer> &vertexBuffer) : _vertexBuffer(vertexBuffer) {
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &_vertexArray);

    glBindVertexArray(_vertexArray);
    vertexBuffer->bind();
    glBindVertexArray(0);
    vertexBuffer->unbind();
}

PointsVAO::~PointsVAO() {
    glDeleteVertexArrays(1, &_vertexArray);
}

void PointsVAO::draw() {
    glBindVertexArray(_vertexArray);
    glDrawArrays(GL_POINTS, 0, _vertexBuffer->size());
    glBindVertexArray(0);
}

} // namespace Lattice
