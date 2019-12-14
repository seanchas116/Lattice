#pragma once
#include "../support/AggregateUtil.hpp"
#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>

namespace Lattice {
namespace GL {

struct AttributeInfo {
    GLenum type;
    int sizePerComponent;
    int count;
};

namespace detail {

template <typename T>
struct GetAttributeInfo;

template <>
struct GetAttributeInfo<float> {
    static constexpr AttributeInfo value = {GL_FLOAT, 4, 1};
};
template <glm::length_t N>
struct GetAttributeInfo<glm::vec<N, float, glm::defaultp>> {
    static constexpr AttributeInfo value = {GL_FLOAT, 4, N};
};

template <typename T, size_t I>
AttributeInfo getAttributeInfoForMember() {
    using MemberType = decltype(AggregateUtil::get<I>(T()));
    return GetAttributeInfo<MemberType>::value;
}

template <typename T, size_t... Is>
std::vector<AttributeInfo> getAttributeInfos(std::index_sequence<Is...>) {
    return {getAttributeInfoForMember<T, Is>()...};
}

} // namespace detail

class AnyVertexBuffer : protected QOpenGLExtraFunctions {
  public:
    AnyVertexBuffer();
    virtual ~AnyVertexBuffer();
    virtual size_t size() const = 0;
    virtual std::vector<AttributeInfo> attributes() const = 0;
    void bind();
    void unbind();

  protected:
    void setVertexData(const void *data, size_t size);

  private:
    GLuint _buffer;
};

template <typename T>
class VertexBuffer final : public AnyVertexBuffer {
  public:
    VertexBuffer() {}

    VertexBuffer(const std::vector<T> &vertices) {
        setVertices(vertices);
    }

    void setVertices(const std::vector<T> &vertices) {
        _size = vertices.size();
        setVertexData(vertices.data(), vertices.size() * sizeof(T));
    }

    size_t size() const override { return _size; }

    std::vector<AttributeInfo> attributes() const override {
        constexpr auto arity = AggregateUtil::aggregate_arity<T>::size();
        return detail::getAttributeInfos<T>(std::make_index_sequence<arity>());
    }

  private:
    size_t _size = 0;
};

} // namespace GL
} // namespace Lattice
