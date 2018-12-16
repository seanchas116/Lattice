#pragma once
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <array>
#include "../support/Pointer.hpp"

namespace Lattice {

class VertexBuffer;

class LineVAO final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(LineVAO)
public:
    using LineStrip = std::vector<uint32_t>;
    using LineAdjacency= std::array<uint32_t, 4>;

    LineVAO();
    LineVAO(const SP<VertexBuffer>& vertexBuffer);
    ~LineVAO();

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
