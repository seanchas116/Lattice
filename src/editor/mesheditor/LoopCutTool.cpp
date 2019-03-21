#include "LoopCutTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../support/Debug.hpp"

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
        int edgeIndex = std::find(nextFaceEdges.begin(), nextFaceEdges.end(), edge) - nextFaceEdges.begin();
        int nextEdgeIndex = (edgeIndex + 2) % nextFaceEdges.size();
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

    qDebug() << edges;
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
