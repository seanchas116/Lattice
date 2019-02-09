#pragma once
#include "../support/Pointer.hpp"
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <array>
#include <vector>
#include "VertexBuffer.hpp"

namespace Lattice {
namespace GL {

class IndexBuffer;

class OldVAO final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(OldVAO)
public:
    using Triangle = std::array<uint32_t, 3>;

    OldVAO();
    OldVAO(const SP<VertexBuffer<Vertex>>& vertexBuffer);
    OldVAO(const SP<VertexBuffer<Vertex>>& vertexBuffer, const std::vector<Triangle>& triangles);
    ~OldVAO();

    auto& vertexBuffer() const { return _vertexBuffer; }

    void draw();
    const std::vector<Triangle>& triangles() const { return _triangles; }
    void setTriangles(const std::vector<Triangle>& triangles);

private:
    SP<VertexBuffer<Vertex>> _vertexBuffer;
    GLuint _vertexArray = 0;
    GLuint _indexBuffer = 0;
    std::vector<Triangle> _triangles;
};

class VAO final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(VAO)
public:
    enum class Primitive {
        Point,
        Line,
        Triangle,
    };
    enum class BufferType {
        PerVertex,
        PerInstance,
    };

    VAO(const std::vector<std::pair<SP<AnyVertexBuffer>, BufferType>>& buffers,
        const SP<IndexBuffer>& indexBuffer);
    VAO(const std::vector<std::pair<SP<AnyVertexBuffer>, BufferType>>& buffers);
    ~VAO();

    void addVertexBuffer(const SP<AnyVertexBuffer>& vertexBuffer);
    void addPerInstanceBuffer(const SP<AnyVertexBuffer>& vertexBuffer);
    void setIndexBuffer(const SP<IndexBuffer>& indexBuffer);

    void draw(Primitive primitive);
    void drawIndexed(Primitive primitive);

private:
    GLuint _vertexArray = 0;
};

}
}
