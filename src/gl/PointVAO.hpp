#pragma once
#include "../support/Pointer.hpp"
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <array>
#include <vector>
#include "../gl/VertexBuffer.hpp"

namespace Lattice::GL {

class PointVAO final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(PointVAO)
public:
    PointVAO();
    PointVAO(const SP<VertexBuffer<Vertex>>& vertexBuffer);
    ~PointVAO();

    auto& vertexBuffer() const { return _vertexBuffer; }

    void draw();

private:
    SP<VertexBuffer<Vertex>> _vertexBuffer;
    GLuint _vertexArray = 0;
};

}
