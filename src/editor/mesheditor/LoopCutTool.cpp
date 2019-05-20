#include "LoopCutTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../mesh/Mesh.hpp"
#include "../../mesh/algorithm/CutEdge.hpp"
#include "../../support/Debug.hpp"
#include "../../support/Distance.hpp"
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/find_if.hpp>

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

void LoopCutTool::mousePressTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    if (!target.edge) {
        return;
    }
    auto edge = *target.edge;
    auto& mesh = *this->mesh();

    Ray<double> mouseRay = event.camera->modelMouseRay(object()->location().matrixToWorld(), event.viewportPos);
    RayRayDistanceSolver distanceSolver(Ray<double>(mesh.ray(edge)), mouseRay);
    double cutPosition = distanceSolver.t0;

    bool isEdgeReverse = false;
    std::vector<std::pair<Mesh::EdgeHandle, bool>> edges;
    Opt<Mesh::FaceHandle> lastFace;

    while (true) {
        edges.push_back({edge, isEdgeReverse});

        auto edgeFaces = mesh.faces(edge) | ranges::to_vector;

        if (edgeFaces.size() != 2) {
            return;
        }
        auto nextFace = (lastFace && *lastFace == edgeFaces[0]) ? edgeFaces[1] : edgeFaces[0];
        lastFace = nextFace;

        auto& nextFaceEdges = mesh.edges(nextFace);

        if (nextFaceEdges.size() != 4) {
            return;
        }
        size_t edgeIndex = ranges::find(nextFaceEdges, edge) - nextFaceEdges.begin();
        size_t nextEdgeIndex = (edgeIndex + 2) % nextFaceEdges.size();
        auto nextEdge = nextFaceEdges[nextEdgeIndex];

        bool edgeDirection = mesh.vertices(nextFace)[edgeIndex] == mesh.vertices(edge)[0];
        bool nextEdgeDirection = mesh.vertices(nextFace)[nextEdgeIndex] == mesh.vertices(nextEdge)[0];
        if (edgeDirection == nextEdgeDirection) {
            isEdgeReverse = !isEdgeReverse;
        }

        if (nextEdge == edges[0].first) {
            // loop found
            break;
        }
        if (ranges::find_if(edges, [&](auto& pair) { return pair.first == nextEdge; }) != edges.end()) {
            // 9-like loop
            return;
        }
        edge = nextEdge;
    }

    std::vector<Mesh::VertexHandle> vertices;
    vertices.reserve(edges.size());
    for (auto& [edge, isReverse] : edges) {
        auto v = Mesh::CutEdge(edge, isReverse ? (1.0 - cutPosition) : cutPosition).perform(mesh);
        vertices.push_back(v);
    }
    for (size_t i = 0; i < vertices.size(); ++i) {
        auto v0 = vertices[i];
        auto v1 = vertices[(i + 1) % vertices.size()];
        mesh.addEdge({v0, v1});
    }

    mesh.deselectAll();
    for (auto& v : vertices) {
        mesh.setSelected(v, true);
    }
    meshEditState()->commitMeshChanged(tr("Loop Cut"));
    emit finished();
}

void LoopCutTool::mouseMoveTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    // TODO
}

void LoopCutTool::mouseReleaseTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    // TODO
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
