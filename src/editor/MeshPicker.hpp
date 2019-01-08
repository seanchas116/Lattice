#pragma once
#include "../document/Mesh.hpp"
#include "../support/Ray.hpp"

namespace Lattice {
namespace Editor {

class MeshPicker {
public:
    MeshPicker(const SP<Document::Mesh>& mesh);

    std::optional<std::pair<SP<Document::MeshFace>, double>> picKFace(const Ray<double>& ray) const;
    std::optional<std::pair<SP<Document::MeshVertex>, double>> pickVertex(const Ray<double>& ray, double distance) const;
    std::optional<std::pair<SP<Document::MeshEdge>, double>> pickEdge(const Ray<double>& ray, double distance) const;

private:
    SP<Document::Mesh> _mesh;
};

} // namespace Viewport
} // namespace Lattice