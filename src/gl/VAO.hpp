#pragma once
#include "../support/Shorthands.hpp"
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <array>
#include <vector>
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

namespace Lattice {
namespace GL {

enum class BufferType {
    PerVertex,
    PerInstance,
};

class VAO final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(VAO)
public:
    VAO();
    VAO(const SP<AnyVertexBuffer>& buffer, const SP<IndexBuffer>& indexBuffer);
    VAO(const SP<AnyVertexBuffer>& buffer, Primitive primitive);
    VAO(const std::vector<std::pair<SP<AnyVertexBuffer>, BufferType>>& buffers, const SP<IndexBuffer>& indexBuffer);
    VAO(const std::vector<std::pair<SP<AnyVertexBuffer>, BufferType>>& buffers, Primitive primitive);
    ~VAO();

    auto& buffers() const { return _buffers; }

    void draw();

private:
    VAO(const std::vector<std::pair<SP<AnyVertexBuffer>, BufferType>>& buffers, const std::optional<SP<IndexBuffer>>& indexBuffer, Primitive primitive);

    std::vector<std::pair<SP<AnyVertexBuffer>, BufferType>> _buffers;
    std::optional<SP<IndexBuffer>> _indexBuffer;
    Primitive _primitive;
    GLuint _vertexArray = 0;
};

}
}
