#pragma once
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "../support/AggregateUtil.hpp"

namespace Lattice::GL {

struct AttributeInfo {
    GLenum type;
    int size;
};

namespace detail {

template <typename T> struct GetAttributeInfo;

template <> struct GetAttributeInfo<float> {
    static constexpr AttributeInfo value = {GL_FLOAT, 1};
};
template <glm::length_t N> struct GetAttributeInfo<glm::vec<N, float, glm::defaultp>> {
    static constexpr AttributeInfo value = {GL_FLOAT, N};
};

template <typename T, size_t I>
AttributeInfo getAttributeInfoForMember() {
    using MemberType = decltype(AggregateUtil::get<I>(T()));
    using MemberTypeRaw = std::remove_cv_t<std::remove_reference_t<MemberType>>;
    return GetAttributeInfo<MemberTypeRaw>::value;
}

template <typename T, size_t... Is>
std::vector<AttributeInfo> getAttributeInfos(std::index_sequence<Is...>) {
    return { getAttributeInfoForMember<T, Is>()... };
}

}

class AnyVertexBuffer {
public:
    virtual ~AnyVertexBuffer();
    virtual std::vector<AttributeInfo> attributes() const = 0;
};

template <typename T>
class VertexBuffer final : public AnyVertexBuffer, protected QOpenGLExtraFunctions {
public:
    VertexBuffer() {
        initializeOpenGLFunctions();
        glGenBuffers(1, &_buffer);
    }
    ~VertexBuffer() override {
        glDeleteBuffers(1, &_buffer);
    }
    void setVertices(const std::vector<T>& vertices) {
        _size = vertices.size();
        glBindBuffer(GL_ARRAY_BUFFER, _buffer);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertices.size() * sizeof(T)), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void bind() {
        glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    }
    void unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    std::vector<AttributeInfo> attributes() const override {
        constexpr auto arity = AggregateUtil::aggregate_arity<T>::size();
        return detail::getAttributeInfos<T>(std::make_index_sequence<arity>());
    }

private:
    size_t _size = 0;
    GLuint _buffer;
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

using StandardVertexBuffer = VertexBuffer<OldVertexBuffer::Vertex>;

} // namespace Lattice
