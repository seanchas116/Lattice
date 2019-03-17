#include "ExtrudeTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../support/Debug.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

Tool::HitTestExclusion ExtrudeTool::hitTestExclusion() const {
    return {};
}

void ExtrudeTool::mousePress(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    auto clickedFragment = target.fragment();
    if (clickedFragment.empty()) {
        return;
    }
    auto fragment = appState()->document()->meshSelection();
    if (!fragment.contains(clickedFragment)) {
        fragment = clickedFragment;
    }

    // TODO: start extruding when dragg started
    appState()->document()->history()->beginChange(tr("Extrude"));
    auto& mesh = item()->mesh();

    _oldToNewVertices.clear();

    auto edges = fragment.edges();
    auto faces = fragment.faces();

    std::unordered_set<SP<Mesh::Edge>> openEdges;
    for (auto& edge : edges) {
        int faceCount = 0;
        for (auto& face : edge->faces()) {
            if (faces.find(face) != faces.end()) {
                ++faceCount;
            }
        }
        if (faceCount <= 1) {
            openEdges.insert(edge);
        }
    }

    for (auto& vertex : fragment.vertices) {
        auto uv = vertex->firstUVPoint(); // TODO: find best uv
        auto newUVPoint = mesh->addUVPoint(mesh->addVertex(vertex->position()), uv->position());
        mesh->addEdge({vertex, newUVPoint->vertex()});
        _oldToNewVertices.insert({vertex, newUVPoint->vertex()});
    }

    for (auto& edge : edges) {
        auto v0 = _oldToNewVertices.at(edge->vertices()[0]);
        auto v1 = _oldToNewVertices.at(edge->vertices()[1]);
        mesh->addEdge({v0, v1});
    }

    for (auto& openEdge : openEdges) {
        bool isReverse = true;
        SP<Mesh::Material> material = mesh->materials()[0];

        for (auto& face : openEdge->faces()) {
            if (faces.find(face) != faces.end()) {
                material = face->material();
                continue;
            }

            for (size_t i = 0; i < face->vertices().size(); ++i) {
                size_t i2 = (i + 1) % face->vertices().size();
                if (face->vertices()[i] == openEdge->vertices()[0] && face->vertices()[i2] == openEdge->vertices()[1]) {
                    isReverse = false;
                    break;
                }
            }
        }

        auto v0 = openEdge->vertices()[0];
        auto v1 = openEdge->vertices()[1];
        auto v2 = _oldToNewVertices.at(openEdge->vertices()[1]);
        auto v3 = _oldToNewVertices.at(openEdge->vertices()[0]);

        // TODO: find best uvpoint
        if (isReverse) {
            mesh->addFace({v0->firstUVPoint(), v1->firstUVPoint(), v2->firstUVPoint(), v3->firstUVPoint()}, material);
        } else {
            mesh->addFace({v3->firstUVPoint(), v2->firstUVPoint(), v1->firstUVPoint(), v0->firstUVPoint()}, material);
        }
    }

    for (auto& face : faces) {
        std::vector<SP<Mesh::UVPoint>> newUVPoints;
        for (auto& uv : face->uvPoints()) {
            auto newUV = _oldToNewVertices.at(uv->vertex());
            newUVPoints.push_back(newUV->firstUVPoint());
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
