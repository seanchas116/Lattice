#include "LoopCutTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../support/Debug.hpp"
#include "../../support/Distance.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

void LoopCutTool::mousePressEvent(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    // TODO

    if (!target.edge) {
        return;
    }
    auto edge = *target.edge;
    auto mesh = object()->mesh();

    Ray<double> mouseRay = event.camera->modelMouseRay(object()->location().matrixToWorld(), event.viewportPos);
    RayRayDistanceSolver distanceSolver(Ray<double>(edge->ray()), mouseRay);
    double cutPosition = distanceSolver.t0;

    bool isEdgeReverse = false;
    std::vector<std::pair<SP<Mesh::Edge>, bool>> edges;
    Opt<SP<Mesh::Face>> lastFace;

    while (true) {
        edges.push_back({edge, isEdgeReverse});

        if (edge->faces().size() != 2) {
            return;
        }
        auto nextFace = (lastFace == edge->faces()[0]) ? edge->faces()[1] : edge->faces()[0];
        lastFace = nextFace;

        if (nextFace->edges().size() != 4) {
            return;
        }
        auto& nextFaceEdges = nextFace->edges();
        size_t edgeIndex = std::find(nextFaceEdges.begin(), nextFaceEdges.end(), edge) - nextFaceEdges.begin();
        size_t nextEdgeIndex = (edgeIndex + 2) % nextFaceEdges.size();
        auto nextEdge = nextFaceEdges[nextEdgeIndex];

        bool edgeDirection = nextFace->vertices()[edgeIndex] == edge->vertices()[0];
        bool nextEdgeDirection = nextFace->vertices()[nextEdgeIndex] == nextEdge->vertices()[0];
        if (edgeDirection == nextEdgeDirection) {
            isEdgeReverse = !isEdgeReverse;
        }

        if (nextEdge == edges[0].first) {
            // loop found
            break;
        }
        if (std::find_if(edges.begin(), edges.end(), [&](auto& pair) { return pair.first == nextEdge; }) != edges.end()) {
            // 9-like loop
            return;
        }
        edge = nextEdge;
    }

    std::vector<SP<Mesh::Vertex>> vertices;
    vertices.reserve(edges.size());
    for (auto& [edge, isReverse] : edges) {
        auto v = mesh->cutEdge(edge, isReverse ? (1.0 - cutPosition) : cutPosition);
        vertices.push_back(v);
    }
    for (size_t i = 0; i < vertices.size(); ++i) {
        auto v0 = vertices[i];
        auto v1 = vertices[(i + 1) % vertices.size()];
        mesh->addEdge({v0, v1});
    }

    Mesh::MeshFragment selection;
    for (auto& v : vertices) {
        selection.vertices.insert(v);
    }
    appState()->document()->setMeshSelection(selection);
    emit finished();
}

void LoopCutTool::mouseMoveEvent(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    // TODO
}

void LoopCutTool::mouseReleaseEvent(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    // TODO
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
