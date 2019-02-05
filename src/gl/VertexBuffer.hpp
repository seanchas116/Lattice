#pragma once
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Lattice::GL {

struct AttributeInfo {
    GLenum type;
    int size;
};

class AnyVertexBuffer {
public:
    virtual ~AnyVertexBuffer();
    virtual std::vector<AttributeInfo> attributes() = 0;
};

class OldVertexBuffer final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(OldVertexBuffer)
public:
    struct Vertex {
        glm::vec3 position {0};
        glm::vec2 texCoord {0};
        glm::vec3 normal {0};
        glm::vec3 color {0};
    };
    enum VertexFlags {
        VertexSelected = 1,
    };

    OldVertexBuffer();
    OldVertexBuffer(const std::vector<Vertex>& vertices);
    ~OldVertexBuffer();

    void setVertices(const std::vector<Vertex>& vertices);

    size_t size() const { return _size; }

    void bind();
    void unbind();

private:
    friend class VAO;
    size_t _size = 0;
    GLuint _buffer;
};

} // namespace Lattice
