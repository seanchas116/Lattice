#pragma once
#include "../support/Pointer.hpp"
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <array>
#include <vector>

namespace Lattice {

class VertexBuffer;

class PointsVAO final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(PointsVAO)
public:
    PointsVAO();
    PointsVAO(const SP<VertexBuffer>& vertexBuffer);
    ~PointsVAO();

    const SP<VertexBuffer>& vertexBuffer() const { return _vertexBuffer; }

    void draw();

private:
    SP<VertexBuffer> _vertexBuffer;
    GLuint _vertexArray = 0;
};

}
