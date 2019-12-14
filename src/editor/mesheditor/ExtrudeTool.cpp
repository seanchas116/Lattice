#include "ExtrudeTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../state/Preferences.hpp"
#include "../../support/Debug.hpp"
#include "../../support/SetUtil.hpp"
#include <meshlib/Mesh.hpp>
#include <meshlib/algorithm/Extrude.hpp>

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

Tool::HitTestExclusion ExtrudeTool::hitTestExclusion() const {
    return {};
}

void ExtrudeTool::mousePressTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    auto &mesh = *this->mesh();

    auto clickedVertices = target.vertices(mesh);
    if (clickedVertices.empty()) {
        return;
    }
    auto vertices = mesh.selectedVertices() | ranges::to<std::unordered_set<meshlib::VertexHandle>>();

    if (!includes(vertices, clickedVertices)) {
        vertices = clickedVertices;
    }

    _dragStarted = false;
    _initWorldPos = event.worldPos();
    _initViewportPos = event.viewportPos;
    _vertices = vertices | ranges::to_vector;
}

void ExtrudeTool::mouseMoveTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target);

    auto &mesh = *this->mesh();

    if (_vertices.empty()) {
        return;
    }

    if (!_dragStarted) {
        if (distance(_initViewportPos, dvec2(event.viewportPos.xy)) < State::Preferences::shared().moveThreshold()) {
            return;
        }

        _newVertices = meshlib::extrude(mesh, _vertices);

        auto faces = mesh.faces(_newVertices);
        if (faces.empty()) {
            _useGuide = false;
        } else {
            _useGuide = true;
            glm::dvec3 normal{0};
            for (auto &face : faces) {
                normal += mesh.calculateNormal(face);
            }
            _guideDirection = glm::normalize(normal);
        }

        mesh.deselectAll();

        for (auto &&v : _newVertices) {
            mesh.setSelected(v, true);
        }

        _dragStarted = true;
        meshEditState()->notifyMeshChanged();
        return;
    }

    dvec3 offset = event.worldPos() - _initWorldPos;
    if (_useGuide) {
        offset = glm::dot(offset, _guideDirection) * _guideDirection;
    }

    for (size_t i = 0; i < _vertices.size(); ++i) {
        auto vertex = _vertices[i];
        auto newVertex = _newVertices[i];

        auto newPos = mesh.position(vertex) + vec3(offset);
        mesh.setPosition(newVertex, newPos);
    }

    meshEditState()->notifyMeshChanged();
}

void ExtrudeTool::mouseReleaseTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target);
    Q_UNUSED(event);

    _vertices.clear();
    _newVertices.clear();

    if (_dragStarted) {
        _dragStarted = false;
        meshEditState()->commitMeshChanged(tr("Extrude"));
        emit finished();
    }
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
