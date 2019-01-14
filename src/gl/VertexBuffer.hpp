#pragma once
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Lattice::GL {

class VertexBuffer final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(VertexBuffer)
public:
    struct Vertex {
        glm::vec3 position {0};
        glm::vec2 texCoord {0};
        glm::vec3 normal {0};
        uint32_t flags {0};
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
