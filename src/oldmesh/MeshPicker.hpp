#pragma once
#include "Mesh.hpp"
#include "../support/Ray.hpp"
#include "../support/Camera.hpp"

namespace Lattice {
namespace OldMesh {

class MeshPicker {
public:
    MeshPicker(const SP<Mesh>& mesh);

    Opt<std::pair<SP<Face>, double>> pickFace(const glm::dmat4 &modelToWorld, const SP<Camera>& camera, glm::dvec2 viewportPos) const;
    Opt<std::pair<SP<Vertex>, double>> pickVertex(const glm::dmat4 &modelToWorld, const SP<Camera> &camera, glm::dvec2 viewportPos, double distance) const;
    Opt<std::pair<SP<Edge>, double>> pickEdge(const glm::dmat4 &modelToWorld, const SP<Camera> &camera, glm::dvec2 viewportPos, double distance) const;

private:
    SP<Mesh> _mesh;
};

} // namespace Mesh
} // namespace Lattice