#pragma once
#include "../support/Pointer.hpp"
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <array>
#include <vector>

namespace Lattice::GL {

class OldVertexBuffer;

class VAO final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(VAO)
public:
    using Triangle = std::array<uint32_t, 3>;

    VAO();
    VAO(const SP<OldVertexBuffer>& vertexBuffer);
    VAO(const SP<OldVertexBuffer>& vertexBuffer, const std::vector<Triangle>& triangles);
    ~VAO();

    const SP<OldVertexBuffer>& vertexBuffer() const { return _vertexBuffer; }

    void draw();
    const std::vector<Triangle>& triangles() const { return _triangles; }
    void setTriangles(const std::vector<Triangle>& triangles);

private:
    SP<OldVertexBuffer> _vertexBuffer;
    GLuint _vertexArray = 0;
    GLuint _indexBuffer = 0;
    std::vector<Triangle> _triangles;
};

}
