#pragma once
#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "../support/AggregateUtil.hpp"

namespace Lattice::GL {

struct AttributeInfo {
    GLenum type;
    int sizePerComponent;
    int count;
};

namespace detail {

template <typename T> struct GetAttributeInfo;

template <> struct GetAttributeInfo<float> {
    static constexpr AttributeInfo value = {GL_FLOAT, 4, 1};
};
template <glm::length_t N> struct GetAttributeInfo<glm::vec<N, float, glm::defaultp>> {
    static constexpr AttributeInfo value = {GL_FLOAT, 4, N};
};

template <typename T, size_t I>
AttributeInfo getAttributeInfoForMember() {
    using MemberType = decltype(AggregateUtil::get<I>(T()));
    return GetAttributeInfo<MemberType>::value;
}

template <typename T, size_t... Is>
std::vector<AttributeInfo> getAttributeInfos(std::index_sequence<Is...>) {
    return { getAttributeInfoForMember<T, Is>()... };
}

}

class AnyVertexBuffer : protected QOpenGLExtraFunctions {
public:
    AnyVertexBuffer();
    virtual ~AnyVertexBuffer();
    virtual std::vector<AttributeInfo> attributes() const = 0;
    void bind();
    void unbind();
protected:
    void setVertices(void* data, size_t size);
private:
    GLuint _buffer;
};

template <typename T>
class VertexBuffer final : public AnyVertexBuffer {
public:
    void setVertices(const std::vector<T>& vertices) {
        _size = vertices.size();
        setVertices(vertices.data(), vertices.size() * sizeof(T));
    }

    size_t size() const { return _size; }

    std::vector<AttributeInfo> attributes() const override {
        constexpr auto arity = AggregateUtil::aggregate_arity<T>::size();
        return detail::getAttributeInfos<T>(std::make_index_sequence<arity>());
    }

private:
    size_t _size = 0;
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
