#pragma once
#include "../mesh/Mesh.hpp"
#include "../support/Ray.hpp"
#include "../support/Camera.hpp"

namespace Lattice {
namespace Editor {

class MeshPicker {
public:
    MeshPicker(const SP<Mesh::Mesh>& mesh);

    Opt<std::pair<SP<Mesh::Face>, double>> pickFace(const glm::dmat4 &modelToWorld, const SP<Camera>& camera, glm::dvec2 screenPos) const;
    Opt<std::pair<SP<Mesh::Vertex>, double>> pickVertex(const glm::dmat4 &modelToWorld, const SP<Camera> &camera, glm::dvec2 screenPos, double distance) const;
    Opt<std::pair<SP<Mesh::Edge>, double>> pickEdge(const glm::dmat4 &modelToWorld, const SP<Camera> &camera, glm::dvec2 screenPos, double distance) const;

private:
    SP<Mesh::Mesh> _mesh;
};

} // namespace Viewport
} // namespace Lattice
