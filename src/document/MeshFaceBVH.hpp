#pragma once
#include "Mesh.hpp"

namespace Lattice {
namespace Document {

class MeshFaceBVH {
public:
    MeshFaceBVH(const SP<Mesh>& mesh);

    std::optional<std::pair<SP<Document::MeshFace>, float>> pick(const Ray<float>& ray) const;

private:
    struct Node {
        Box<float> boundingBox;
        int splitAxis;
        std::array<Node*, 2> children;
        std::vector<SP<MeshFace>> faces;
        std::optional<std::pair<SP<MeshFace>, float>> pick(const Ray<float> &ray) const;
    };
    std::vector<Node> nodes;
};

} // namespace Document
} // namespace Lattice
