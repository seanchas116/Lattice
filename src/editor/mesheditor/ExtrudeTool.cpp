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

void ExtrudeTool::mousePressTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    auto clickedFragment = target.fragment();
    if (clickedFragment.empty()) {
        return;
    }
    _fragment = appState()->document()->meshSelection();
    if (!_fragment.contains(clickedFragment)) {
        _fragment = clickedFragment;
    }
    _dragStarted = false;

    _initWorldPos = event.worldPos();
    _initViewportPos = event.viewportPos;

    _initPositions.clear();
    for (auto& v : _fragment.vertices) {
        _initPositions[v] = v->position();
    }
}

void ExtrudeTool::mouseMoveTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target);

    if (_fragment.empty()) {
        return;
    }

    if (!_dragStarted) {
        if (distance(_initViewportPos, dvec2(event.viewportPos.xy)) < appState()->preferences()->moveThreshold()) {
            return;
        }

        appState()->document()->history()->beginChange(tr("Extrude"));
        auto& mesh = object()->oldMesh();

        _oldToNewUVPoints.clear();

        auto edges = _fragment.edges();
        auto faces = _fragment.faces();

        std::unordered_set<SP<OldMesh::Edge>> openEdges;
        for (auto& edge : edges) {
            int faceCount = 0;
            for (auto& face : edge->faces()) {
                if (faces.find(face->sharedFromThis()) != faces.end()) {
                    ++faceCount;
                }
            }
            if (faceCount <= 1) {
                openEdges.insert(edge);
            }
        }

        for (auto& vertex : _fragment.vertices) {
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
            SP<OldMesh::Material> material = mesh->materials()[0];

            for (auto& face : openEdge->faces()) {
                if (faces.find(face->sharedFromThis()) != faces.end()) {
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
            std::vector<SP<OldMesh::UVPoint>> newUVPoints;
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
            glm::dvec3 normal {0};
            for (auto& face : faces) {
                normal += face->normal();
            }
            _guideDirection = glm::normalize(normal);
        }

        OldMesh::MeshFragment selection;
        for (auto& [oldUV, newUV] : _oldToNewUVPoints) {
            selection.vertices.insert(newUV->vertex());
        }
        appState()->document()->setMeshSelection(selection);

        _dragStarted = true;
        return;
    }

    auto offset = event.worldPos() - _initWorldPos;
    if (_useGuide) {
        offset = glm::dot(offset, _guideDirection) * _guideDirection;
    }

    std::unordered_map<SP<OldMesh::Vertex>, dvec3> newPositions;

    for (auto& [oldUV, newUV] : _oldToNewUVPoints) {
        newPositions[newUV->vertex()] = _initPositions[oldUV->vertex()] + offset;
    }

    object()->oldMesh()->setPosition(newPositions);
}

void ExtrudeTool::mouseReleaseTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);

    _fragment = {};
    _initPositions.clear();
    _vertexToUV.clear();
    _oldToNewUVPoints.clear();

    if (_dragStarted) {
        _dragStarted = false;
        emit finished();
    }
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
