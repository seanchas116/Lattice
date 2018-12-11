#pragma once
#include "../support/Pointer.hpp"
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <array>
#include <vector>

namespace Lattice {

class VertexBuffer;

class Mesh final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(Mesh)
public:
    using Triangle = std::array<uint32_t, 3>;

    Mesh();
    Mesh(const SP<VertexBuffer>& vertexBuffer);
    Mesh(const SP<VertexBuffer>& vertexBuffer, const std::vector<Triangle>& triangles);
    ~Mesh();

    const SP<VertexBuffer>& vertexBuffer() const { return _vertexBuffer; }

    void draw();
    const std::vector<Triangle>& triangles() const { return _triangles; }
    void setTriangles(const std::vector<Triangle>& triangles);

private:
    SP<VertexBuffer> _vertexBuffer;
    GLuint _vertexArray = 0;
    GLuint _indexBuffer = 0;
    std::vector<Triangle> _triangles;
};

}
