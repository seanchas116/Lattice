#pragma once
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <array>
#include "../support/Pointer.hpp"

namespace Lattice {

class VertexBuffer;

class LineMesh final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(LineMesh)
public:
    using LineStrip = std::vector<uint32_t>;
    using LineAdjacency= std::array<uint32_t, 4>;

    LineMesh();
    LineMesh(const SP<VertexBuffer>& vertexBuffer);
    ~LineMesh();

    const SP<VertexBuffer>& vertexBuffer() const { return _vertexBuffer; }

    void draw();
    void setLineStrips(const std::vector<LineStrip> &strips);
    void setLineAdjacencies(const std::vector<LineAdjacency>& lines);

private:
    SP<VertexBuffer> _vertexBuffer;
    GLuint _vertexArray = 0;
    GLuint _indexBuffer = 0;
    int _lineCount = 0;
};

}
