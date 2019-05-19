#pragma once

#include <cstdint>
#include <functional>

namespace Lattice {
namespace Mesh {

template <typename Tag>
struct Handle {
    Handle() : index(0) {}
    explicit Handle(uint32_t index) : index(index) {}
    bool operator==(const Handle& other) const { return index == other.index; }
    bool operator!=(const Handle& other) const { return !operator==(other); }
    uint32_t index;
};

struct VertexHandleTag {};
struct UVPointHandleTag {};
struct EdgeHandleTag {};
struct FaceHandleTag {};
struct MaterialHandleTag {};

using VertexHandle = Handle<VertexHandleTag>;
using UVPointHandle = Handle<UVPointHandleTag>;
using EdgeHandle = Handle<EdgeHandleTag>;
using FaceHandle = Handle<FaceHandleTag>;
using MaterialHandle = Handle<MaterialHandleTag>;

} // namespace NewMesh
} // namespace Lattice

namespace std {

template <typename Tag>
struct hash<Lattice::Mesh::Handle<Tag>> {
    size_t operator()(const Lattice::Mesh::Handle<Tag>& value) const {
        return std::hash<uint32_t>()(value.index);
    }
};

}
