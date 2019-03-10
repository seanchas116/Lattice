#pragma once
#include "../mesh/Mesh.hpp"
#include "../support/Ray.hpp"
#include "../support/Camera.hpp"

namespace Lattice {
namespace Editor {

class MeshPicker {
public:
    MeshPicker(const SP<Document::Mesh>& mesh);

    Opt<std::pair<SP<Document::MeshFace>, double>> pickFace(const glm::dmat4 &modelToWorld, const SP<Camera>& camera, glm::dvec2 screenPos) const;
    Opt<std::pair<SP<Document::MeshVertex>, double>> pickVertex(const glm::dmat4 &modelToWorld, const SP<Camera> &camera, glm::dvec2 screenPos, double distance) const;
    Opt<std::pair<SP<Document::MeshEdge>, double>> pickEdge(const glm::dmat4 &modelToWorld, const SP<Camera> &camera, glm::dvec2 screenPos, double distance) const;

private:
    SP<Document::Mesh> _mesh;
};

} // namespace Viewport
} // namespace Lattice
