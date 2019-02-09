#include "PointVAO.hpp"
#include "VertexBuffer.hpp"

namespace Lattice::GL {

PointVAO::PointVAO() : PointVAO(makeShared<VertexBuffer<Vertex>>()) {
}

PointVAO::PointVAO(const SP<VertexBuffer<Vertex>> &vertexBuffer) : _vertexBuffer(vertexBuffer) {
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
    glDrawArrays(GL_POINTS, 0, _vertexBuffer->size());
    glBindVertexArray(0);
}

} // namespace Lattice
