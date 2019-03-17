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

    _oldToNewUVPoints.clear();

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
        _vertexToUV.insert({vertex, uv});
        _vertexToUV.insert({newUVPoint->vertex(), newUVPoint});
        _oldToNewUVPoints.insert({uv, newUVPoint});
    }

    for (auto& edge : edges) {
        auto uv0 = _oldToNewUVPoints.at(_vertexToUV.at(edge->vertices()[0]));
        auto uv1 = _oldToNewUVPoints.at(_vertexToUV.at(edge->vertices()[1]));
        mesh->addEdge({uv0->vertex(), uv1->vertex()});
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

        auto uv0 = _vertexToUV.at(openEdge->vertices()[0]);
        auto uv1 = _vertexToUV.at(openEdge->vertices()[1]);
        auto uv2 = _oldToNewUVPoints.at(uv1);
        auto uv3 = _oldToNewUVPoints.at(uv0);

        if (isReverse) {
            mesh->addFace({uv0, uv1, uv2, uv3}, material);
        } else {
            mesh->addFace({uv3, uv2, uv1, uv0}, material);
        }
    }

    for (auto& face : faces) {
        std::vector<SP<Mesh::UVPoint>> newUVPoints;
        for (auto& uv : face->uvPoints()) {
            auto newUV = _oldToNewUVPoints.at(uv);
            newUVPoints.push_back(newUV);
        }
        mesh->addFace(newUVPoints, face->material());
        mesh->removeFace(face);
    }

    if (faces.empty()) {
        _useGuide = false;
    } else {
        _useGuide = true;
        glm::vec3 normal {0};
        for (auto& face : faces) {
            normal += face->normal();
        }
        _guideDirection = glm::normalize(normal);
    }

    Mesh::MeshFragment selection;
    for (auto& [oldUV, newUV] : _oldToNewUVPoints) {
        _initPositions[oldUV->vertex()] = oldUV->vertex()->position();
        selection.vertices.insert(newUV->vertex());
    }
    _initWorldPos = event.worldPos();
    appState()->document()->setMeshSelection(selection);
}

void ExtrudeTool::mouseMove(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target);
    if (_oldToNewUVPoints.empty()) {
        return;
    }

    glm::vec3 offset = glm::vec3(event.worldPos()) - _initWorldPos;
    if (_useGuide) {
        offset = glm::dot(offset, _guideDirection) * _guideDirection;
    }

    std::unordered_map<SP<Mesh::Vertex>, vec3> newPositions;

    for (auto& [oldUV, newUV] : _oldToNewUVPoints) {
        newPositions[newUV->vertex()] = _initPositions[oldUV->vertex()] + offset;
    }

    item()->mesh()->setPositions(newPositions);
}

void ExtrudeTool::mouseRelease(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);

    _oldToNewUVPoints.clear();
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
