#include "ExtrudeTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../support/Debug.hpp"
#include "../../mesh/Mesh.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

namespace {

template <typename T>
bool set_includes(const std::unordered_set<T>& set, const std::unordered_set<T>& otherSet) {
    for (auto& value : otherSet) {
        if (set.find(value) == set.end()) {
            return false;
        }
    }
    return true;
}

}

Tool::HitTestExclusion ExtrudeTool::hitTestExclusion() const {
    return {};
}

void ExtrudeTool::mousePressTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    auto& mesh = *this->mesh();

    auto clickedVertices = target.vertices();
    if (clickedVertices.empty()) {
        return;
    }
    _vertices = mesh.vertices() | ranges::view::filter([&] (auto handle) { return mesh.isSelected(handle); }) | ranges::to_<std::unordered_set<Mesh::VertexHandle>>();

    if (!set_includes(_vertices, clickedVertices)) {
        _vertices = clickedVertices;
    }
    _dragStarted = false;

    _initWorldPos = event.worldPos();
    _initViewportPos = event.viewportPos;

    _initPositions.clear();
    for (auto& v : _vertices) {
        _initPositions[v] = mesh.position(v);
    }
}

void ExtrudeTool::mouseMoveTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target);

    auto& mesh = *this->mesh();

    if (_vertices.empty()) {
        return;
    }

    if (!_dragStarted) {
        if (distance(_initViewportPos, dvec2(event.viewportPos.xy)) < appState()->preferences()->moveThreshold()) {
            return;
        }

        appState()->document()->history()->beginChange(tr("Extrude"));

        _oldToNewUVPoints.clear();

        std::unordered_set<Mesh::EdgeHandle> edges;
        for (auto v : _vertices) {
            for (auto e : mesh.edges(v)) {
                edges.insert(e);
            }
        }
        std::unordered_set<Mesh::FaceHandle> faces;
        for (auto v : _vertices) {
            for (auto f : mesh.faces(v)) {
                faces.insert(f);
            }
        }

        std::unordered_set<Mesh::EdgeHandle> openEdges;
        for (auto& edge : edges) {
            int faceCount = 0;
            for (auto& face : mesh.faces(edge)) {
                if (faces.find(face) != faces.end()) {
                    ++faceCount;
                }
            }
            if (faceCount <= 1) {
                openEdges.insert(edge);
            }
        }

        for (auto vertex : _vertices) {
            auto uv = mesh.uvPoints(vertex).front(); // TODO: find best uv
            auto newUVPoint = mesh.addUVPoint(mesh.addVertex(mesh.position(vertex)), mesh.uv(uv));
            mesh.addEdge(vertex, mesh.vertex(newUVPoint), false);
            _vertexToUV.insert({vertex, uv});
            _vertexToUV.insert({mesh.vertex(newUVPoint), newUVPoint});
            _oldToNewUVPoints.insert({uv, newUVPoint});
        }

        for (auto& edge : edges) {
            auto uv0 = _oldToNewUVPoints.at(_vertexToUV.at(mesh.vertices(edge)[0]));
            auto uv1 = _oldToNewUVPoints.at(_vertexToUV.at(mesh.vertices(edge)[1]));
            mesh.addEdge(mesh.vertex(uv0), mesh.vertex(uv1), false);
        }

        for (auto& openEdge : openEdges) {
            bool isReverse = true;
            uint32_t material = 0;

            for (auto& face : mesh.faces(openEdge)) {
                if (faces.find(face) != faces.end()) {
                    material = mesh.material(face);
                    continue;
                }

                for (size_t i = 0; i < mesh.vertices(face).size(); ++i) {
                    size_t i2 = (i + 1) % mesh.vertices(face).size();
                    if (mesh.vertices(face)[i] == mesh.vertices(openEdge)[0] && mesh.vertices(face)[i2] == mesh.vertices(openEdge)[1]) {
                        isReverse = false;
                        break;
                    }
                }
            }

            auto uv0 = _vertexToUV.at(mesh.vertices(openEdge)[0]);
            auto uv1 = _vertexToUV.at(mesh.vertices(openEdge)[1]);
            auto uv2 = _oldToNewUVPoints.at(uv1);
            auto uv3 = _oldToNewUVPoints.at(uv0);

            if (isReverse) {
                mesh.addFace({uv0, uv1, uv2, uv3}, material);
            } else {
                mesh.addFace({uv3, uv2, uv1, uv0}, material);
            }
        }

        for (auto& face : faces) {
            std::vector<Mesh::UVPointHandle> newUVPoints;
            for (auto& uv : mesh.uvPoints(face)) {
                auto newUV = _oldToNewUVPoints.at(uv);
                newUVPoints.push_back(newUV);
            }
            mesh.addFace(newUVPoints, mesh.material(face));
            mesh.removeFace(face);
        }

        // TODO: calculate normal
        /*
        if (faces.empty()) {
            _useGuide = false;
        } else {
            _useGuide = true;
            glm::dvec3 normal {0};
            for (auto& face : faces) {
                normal += face->normal();
            }
            _guideDirection = glm::normalize(normal);
        }*/

        mesh.clearSelections();

        for (auto& [oldUV, newUV] : _oldToNewUVPoints) {
            mesh.setSelected(mesh.vertex(newUV), true);
        }

        _dragStarted = true;
        emit meshChanged();
        return;
    }

    auto offset = event.worldPos() - _initWorldPos;
    if (_useGuide) {
        offset = glm::dot(offset, _guideDirection) * _guideDirection;
    }

    std::unordered_map<SP<OldMesh::Vertex>, dvec3> newPositions;

    for (auto& [oldUV, newUV] : _oldToNewUVPoints) {
        auto newPos = _initPositions[mesh.vertex(oldUV)] + offset;
        mesh.setPosition(mesh.vertex(newUV), newPos);
    }

    object()->oldMesh()->setPosition(newPositions);
     emit meshChanged();
}

void ExtrudeTool::mouseReleaseTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);

    _vertices.clear();
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
