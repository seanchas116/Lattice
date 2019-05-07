#include "CutEdge.hpp"
#include <QtGlobal>

using namespace glm;

namespace Lattice {
namespace Mesh {

VertexHandle CutEdge::perform(Mesh &mesh) const {
    auto pos = mesh.ray(edge).at(t);

    auto uv = mesh.addUVPoint(mesh.addVertex(pos), dvec2(0)); // TODO: Use better UV position

    mesh.addEdge(mesh.vertices(edge)[0], mesh.vertex(uv), false);
    mesh.addEdge(mesh.vertex(uv), mesh.vertices(edge)[1], false);

    auto faces = mesh.faces(edge) | ranges::to_vector;
    for (auto& face : faces) {
        std::vector<UVPointHandle> newFaceUVPoints;
        auto& faceUVPoints = mesh.uvPoints(face);
        for (size_t i = 0; i < faceUVPoints.size(); ++i) {
            auto uv0 = faceUVPoints[i];
            auto uv1 = faceUVPoints[(i + 1) % faceUVPoints.size()];

            newFaceUVPoints.push_back(uv0);

            // TODO: create separate uvpoint if possible when uv is split at the edge
            if (mesh.vertex(uv0) == mesh.vertices(edge)[0] && mesh.vertex(uv1) == mesh.vertices(edge)[1]) {
                newFaceUVPoints.push_back(uv);
            } else if (mesh.vertex(uv1) == mesh.vertices(edge)[0] && mesh.vertex(uv0) == mesh.vertices(edge)[1]) {
                newFaceUVPoints.push_back(uv);
            }
        }

        mesh.addFace(newFaceUVPoints, mesh.material(face));
    }
    for (auto& face : faces) {
        mesh.removeFace(face);
    }
    mesh.removeEdge(edge);

    return mesh.vertex(uv);
}

} // namespace Mesh
} // namespace Lattice
