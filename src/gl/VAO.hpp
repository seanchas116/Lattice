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

enum class Primitive {
    Point,
    Line,
    Triangle,
};

enum class BufferType {
    PerVertex,
    PerInstance,
};

class VAO final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(VAO)
public:
    VAO(const std::vector<std::pair<SP<AnyVertexBuffer>, BufferType>>& buffers,
        const std::optional<SP<IndexBuffer>>& indexBuffer = std::nullopt);
    ~VAO();

    void draw(Primitive primitive);

private:
    std::vector<std::pair<SP<AnyVertexBuffer>, BufferType>> _buffers;
    std::optional<SP<IndexBuffer>> _indexBuffer;
    GLuint _vertexArray = 0;
};

}
}
