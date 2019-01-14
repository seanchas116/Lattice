#pragma once
#include "../document/Mesh.hpp"
#include "../support/Ray.hpp"
#include "../support/Camera.hpp"

namespace Lattice {
namespace Editor {

class MeshPicker {
public:
    MeshPicker(const glm::dmat4 &transform, const SP<Document::Mesh>& mesh);

    std::optional<std::pair<SP<Document::MeshFace>, double>> picKFace(const Camera& camera, glm::dvec2 screenPos) const;
    std::optional<std::pair<SP<Document::MeshVertex>, double>> pickVertex(const Ray<double>& ray, double distance) const;
    std::optional<std::pair<SP<Document::MeshEdge>, double>> pickEdge(const Ray<double>& ray, double distance) const;

    glm::dmat4 matrix() const { return _matrix; }
    void setMatrix(const glm::dmat4 &matrix) { _matrix = matrix; }

private:
    glm::dmat4 _matrix;
    SP<Document::Mesh> _mesh;
};

} // namespace Viewport
} // namespace Lattice
