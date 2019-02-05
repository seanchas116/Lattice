#pragma once
#include "../support/Pointer.hpp"
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <array>
#include <vector>

namespace Lattice::GL {

class OldVertexBuffer;

class PointVAO final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(PointVAO)
public:
    PointVAO();
    PointVAO(const SP<OldVertexBuffer>& vertexBuffer);
    ~PointVAO();

    const SP<OldVertexBuffer>& vertexBuffer() const { return _vertexBuffer; }

    void draw();

private:
    SP<OldVertexBuffer> _vertexBuffer;
    GLuint _vertexArray = 0;
};

}
