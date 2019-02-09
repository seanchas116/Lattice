#include "VAO.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include <array>
#include <glm/glm.hpp>

namespace Lattice::GL {

OldVAO::OldVAO() : OldVAO(makeShared<VertexBuffer<Vertex>>()) {
}

OldVAO::OldVAO(const SP<VertexBuffer<Vertex> > &vertexBuffer) : _vertexBuffer(vertexBuffer) {
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

OldVAO::OldVAO(const SP<VertexBuffer<Vertex> > &vertexBuffer, const std::vector<OldVAO::Triangle> &triangles) : OldVAO(vertexBuffer) {
    setTriangles(triangles);
}

OldVAO::~OldVAO() {
    glDeleteVertexArrays(1, &_vertexArray);
    glDeleteBuffers(1, &_indexBuffer);
}

void OldVAO::draw() {
    glBindVertexArray(_vertexArray);
    glDrawElements(GL_TRIANGLES, int(_triangles.size() * 3), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void OldVAO::setTriangles(const std::vector<Triangle> &triangles) {
    _triangles = triangles;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(triangles.size() * sizeof(Triangle)), triangles.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

VAO::VAO(const std::vector<std::pair<SP<AnyVertexBuffer>, VAO::BufferType> > &buffers, const std::optional<SP<IndexBuffer>> &indexBuffer) {
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &_vertexArray);
    glBindVertexArray(_vertexArray);

    for (auto& [buffer, type] : buffers) {
        buffer->bind();
    }
    if (indexBuffer) {
        indexBuffer->get()->bind();
    }

    glBindVertexArray(0);

    for (auto& [buffer, type] : buffers) {
        buffer->unbind();
    }
    if (indexBuffer) {
        indexBuffer->get()->unbind();
    }
}

VAO::~VAO() {
    glDeleteVertexArrays(1, &_vertexArray);
}

void VAO::draw(VAO::Primitive primitive) {
    if (_indexBuffer) {
        auto indexCount = GLsizei(_indexBuffer->get()->size());

        switch (primitive) {
        case Primitive::Point:
            glDrawElements(GL_POINTS, indexCount, GL_UNSIGNED_INT, nullptr);
            break;
        case Primitive::Line:
            glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, nullptr);
            break;
        case Primitive::Triangle:
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
            break;
        }
    } else {
        auto bufferSize = GLsizei(_buffers[0].first->size());

        switch (primitive) {
        case Primitive::Point:
            glDrawArrays(GL_POINTS, 0, bufferSize);
            break;
        case Primitive::Line:
            glDrawArrays(GL_LINES, 0, bufferSize);
            break;
        case Primitive::Triangle:
            glDrawArrays(GL_TRIANGLES, 0, bufferSize);
            break;
        }
    }
}

}
