#pragma once

#include <cstdint>
#include <functional>

namespace Lattice {
namespace Mesh {

template <typename TDerived>
struct Handle {
    explicit Handle(uint32_t index) : index(index) {}
    bool operator==(const TDerived& other) const { return index == other.index; }
    bool operator!=(const TDerived& other) const { return !operator==(other); }
    uint32_t index;
};

struct VertexHandle : public Handle<VertexHandle> {
    VertexHandle() : Handle(0) {}
    explicit VertexHandle(uint32_t index) : Handle(index) {}
};

struct UVPointHandle : public Handle<UVPointHandle> {
    UVPointHandle() : Handle(0) {}
    explicit UVPointHandle(uint32_t index) : Handle(index) {}
};

struct EdgeHandle : public Handle<EdgeHandle> {
    EdgeHandle() : Handle(0) {}
    EdgeHandle(uint32_t index) : Handle(index) {}
};

struct FaceHandle : public Handle<FaceHandle> {
    FaceHandle() : Handle(0) {}
    FaceHandle(uint32_t index) : Handle(index) {}
};

} // namespace NewMesh
} // namespace Lattice

namespace std {

template <>
struct hash<Lattice::Mesh::VertexHandle> {
    size_t operator()(const Lattice::Mesh::VertexHandle& value) const {
        return std::hash<uint32_t>()(value.index);
    }
};

}
