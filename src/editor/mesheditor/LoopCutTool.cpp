#include "LoopCutTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../support/Debug.hpp"
#include "../../support/Distance.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

void LoopCutTool::mousePress(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    // TODO

    if (!target.edge) {
        return;
    }
    auto edge = *target.edge;
    auto mesh = item()->mesh();

    std::vector<SP<Mesh::Edge>> edges;
    Opt<SP<Mesh::Face>> lastFace;

    while (true) {
        edges.push_back(edge);

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

        if (nextEdge == edges[0]) {
            // loop found
            break;
        }
        if (std::find(edges.begin(), edges.end(), nextEdge) != edges.end()) {
            // 9-like loop
            return;
        }
        edge = nextEdge;
    }

    Ray<double> mouseRay = event.camera->modelMouseRay(item()->location().matrixToWorld(), event.screenPos);
    RayRayDistanceSolver distanceSolver(Ray<double>(edge->ray()), mouseRay);
    double cutPosition = distanceSolver.t0;

    std::vector<SP<Mesh::Vertex>> vertices;
    vertices.reserve(edges.size());
    for (auto& e : edges) {
        auto v = mesh->cutEdge(e, cutPosition);
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
}

void LoopCutTool::mouseMove(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    // TODO
}

void LoopCutTool::mouseRelease(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    // TODO
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
