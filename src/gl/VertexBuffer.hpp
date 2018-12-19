#pragma once
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Lattice {

class VertexBuffer final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(VertexBuffer)
public:
    struct Vertex {
        glm::vec3 position;
        glm::vec2 texCoord;
        glm::vec3 normal;
    };

    VertexBuffer();
    VertexBuffer(const std::vector<Vertex>& vertices);
    ~VertexBuffer();

    const std::vector<Vertex>& vertices() const { return _vertices; }
    void setVertices(const std::vector<Vertex>& vertices);

    size_t size() const { return _vertices.size(); }

    void bind();
    void unbind();

private:
    friend class VAO;
    std::vector<Vertex> _vertices;
    GLuint _buffer;
};

} // namespace Lattice
