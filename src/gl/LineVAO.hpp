#pragma once
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <array>
#include "../support/Pointer.hpp"

namespace Lattice::GL {

class OldVertexBuffer;

class LineVAO final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(LineVAO)
public:
    using LineStrip = std::vector<uint32_t>;
    using Line = std::array<uint32_t, 2>;

    LineVAO();
    LineVAO(const SP<OldVertexBuffer>& vertexBuffer);
    ~LineVAO();

    const SP<OldVertexBuffer>& vertexBuffer() const { return _vertexBuffer; }

    void draw();
    void setLines(const std::vector<Line>& lines);
    void setLineStrips(const std::vector<LineStrip> &strips);

private:
    SP<OldVertexBuffer> _vertexBuffer;
    GLuint _vertexArray = 0;
    GLuint _indexBuffer = 0;
    int _lineCount = 0;
};

}
