#include "ExtrudeTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

Tool::HitTestExclusion ExtrudeTool::hitTestExclusion() const {
    return {};
}

void ExtrudeTool::mousePress(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    if (!target.vertex && !target.edge && !target.face) {
        return;
    }

    // TODO: start extruding when dragg started
    appState()->document()->history()->beginChange(tr("Extrude"));
    auto& mesh = item()->mesh();

    if (target.vertex) {
        // vertex extrude
        auto vertex = *target.vertex;
        auto uv = vertex->firstUVPoint();
        auto newUVPoint = mesh->addUVPoint(mesh->addVertex(vertex->position()), uv->position());
        mesh->addEdge({vertex, newUVPoint->vertex()});
        _oldToNewVertices.insert({vertex, newUVPoint->vertex()});
    } else if (target.edge) {
        // edge extrude
        auto edge = *target.edge;
        auto uv0 = edge->vertices()[0]->firstUVPoint();
        auto uv1 = edge->vertices()[1]->firstUVPoint();
        auto uv2 = mesh->addUVPoint(mesh->addVertex(uv1->vertex()->position()), uv1->position());
        auto uv3 = mesh->addUVPoint(mesh->addVertex(uv0->vertex()->position()), uv0->position());
        mesh->addFace({uv0, uv1, uv2, uv3}, mesh->materials()[0]); // TODO: correct material
        // TODO: correct face orientation

        _oldToNewVertices.insert({uv0->vertex(), uv3->vertex()});
        _oldToNewVertices.insert({uv1->vertex(), uv2->vertex()});
    } else if (target.face) {
        auto face = *target.face;

        std::vector<SP<Mesh::UVPoint>> newUVPoints;
        for (auto& uv : face->uvPoints()) {
            auto newUV = mesh->addUVPoint(mesh->addVertex(uv->vertex()->position()), uv->position());
            newUVPoints.push_back(newUV);
        }

        for (size_t i = 0; i < face->uvPoints().size(); ++i) {
            size_t next = (i + 1) % face->uvPoints().size();
            auto uv0 = face->uvPoints()[i];
            auto uv1 = face->uvPoints()[next];
            auto uv2 = newUVPoints[next];
            auto uv3 = newUVPoints[i];

            mesh->addFace({uv0, uv1, uv2, uv3}, face->material());

            _oldToNewVertices.insert({face->uvPoints()[i]->vertex(), newUVPoints[i]->vertex()});
        }

        mesh->addFace(newUVPoints, face->material());
        mesh->removeFace(face);
    }

    for (auto& [oldVertex, newVertex] : _oldToNewVertices) {
        _initPositions[oldVertex] = oldVertex->position();
    }
    _initWorldPos = event.worldPos();
}

void ExtrudeTool::mouseMove(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target);
    if (_oldToNewVertices.empty()) {
        return;
    }

    auto offset = event.worldPos() - _initWorldPos;

    std::unordered_map<SP<Mesh::Vertex>, vec3> newPositions;

    for (auto& [oldVertex, newVertex] : _oldToNewVertices) {
        newPositions[newVertex] = _initPositions[oldVertex] + offset;
    }

    item()->mesh()->setPositions(newPositions);
}

void ExtrudeTool::mouseRelease(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);

    _oldToNewVertices.clear();
    _initPositions.clear();
}

void ExtrudeTool::hoverEnter(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    // TOOD
}

void ExtrudeTool::hoverLeave(const Tool::EventTarget &target) {
    Q_UNUSED(target);
    // TOOD
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
