#include "PointVAO.hpp"
#include "VertexBuffer.hpp"

namespace Lattice::GL {

PointVAO::PointVAO() : PointVAO(makeShared<VertexBuffer>()) {
}

PointVAO::PointVAO(const SP<VertexBuffer> &vertexBuffer) : _vertexBuffer(vertexBuffer) {
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &_vertexArray);

    glBindVertexArray(_vertexArray);
    vertexBuffer->bind();
    glBindVertexArray(0);
    vertexBuffer->unbind();
}

PointVAO::~PointVAO() {
    glDeleteVertexArrays(1, &_vertexArray);
}

void PointVAO::draw() {
    glBindVertexArray(_vertexArray);
    glDrawArrays(GL_POINTS, 0, _vertexBuffer->vertices.size());
    glBindVertexArray(0);
}

} // namespace Lattice
