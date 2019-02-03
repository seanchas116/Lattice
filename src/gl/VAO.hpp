#pragma once
#include "../support/Pointer.hpp"
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <array>
#include <vector>

namespace Lattice::OldGL {

class VertexBuffer;

class VAO final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(VAO)
public:
    using Triangle = std::array<uint32_t, 3>;

    VAO();
    VAO(const SP<VertexBuffer>& vertexBuffer);
    VAO(const SP<VertexBuffer>& vertexBuffer, const std::vector<Triangle>& triangles);
    ~VAO();

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
