#include "DrawTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

void DrawTool::mousePress(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    auto mesh = item()->mesh();
    if (!_drawnUVPoints.empty()) {
        if (target.vertex && std::find(_drawnUVPoints.begin(), _drawnUVPoints.end(), (*target.vertex)->firstUVPoint()) == _drawnUVPoints.end()) {
            auto targetVertex = *target.vertex;

            mesh->removeVertex(_drawnUVPoints[_drawnUVPoints.size() - 1]->vertex());
            _drawnUVPoints.pop_back();

            // connect to existing vertex
            auto closingPointIt = std::find(_drawnUVPoints.begin(), _drawnUVPoints.end(), targetVertex->firstUVPoint());
            if (closingPointIt != _drawnUVPoints.end()) {
                // create face
                std::vector<SP<Mesh::UVPoint>> points(closingPointIt, _drawnUVPoints.end());
                auto face = mesh->addFace(points, mesh->materials()[0]);

                bool isFaceFore = dot(face->normal(), vec3(event.camera->location().backward())) > 0;
                if (!isFaceFore) {
                    mesh->flipFace(face);
                }

                _drawnUVPoints.clear();
                return;
            }
            auto prevUVPoint = _drawnUVPoints[_drawnUVPoints.size() - 1];
            mesh->addEdge({prevUVPoint->vertex(), targetVertex});
            _drawnUVPoints.push_back(targetVertex->firstUVPoint());

            auto uvPoint = mesh->addUVPoint(mesh->addVertex(targetVertex->position()), vec2(0));
            _drawnUVPoints.push_back(uvPoint);
            mesh->addEdge({targetVertex, uvPoint->vertex()});

        } else {
            // add new vertex
            auto prevUVPoint = _drawnUVPoints[_drawnUVPoints.size() - 1];
            auto [prevPosInScreen, isInScreen] = event.camera->mapWorldToScreen(prevUVPoint->vertex()->position());
            if (!isInScreen) {
                return;
            }
            auto pos = event.camera->mapScreenToWorld(dvec3(event.screenPos, prevPosInScreen.z));

            mesh->setPositions({{prevUVPoint->vertex(), pos}});

            auto uvPoint = mesh->addUVPoint(mesh->addVertex(pos), vec2(0));
            _drawnUVPoints.push_back(uvPoint);
            mesh->addEdge({prevUVPoint->vertex(), uvPoint->vertex()});
        }
    } else {
        if (target.vertex) {
            // start from existing vertex
            auto& vertex = *target.vertex;
            auto point1 = (*vertex->uvPoints().begin())->sharedFromThis();
            auto point2 = mesh->addUVPoint(mesh->addVertex(vertex->position()), vec2(0));
            _drawnUVPoints.push_back(point1);
            _drawnUVPoints.push_back(point2);
            mesh->addEdge({point1->vertex(), point2->vertex()});

        } else {
            // start from new vertex
            // TODO: better depth
            auto [centerInScreen, isCenterInScreen] = event.camera->mapWorldToScreen(vec3(0));
            if (!isCenterInScreen) {
                return;
            }
            auto pos = event.camera->mapScreenToWorld(dvec3(event.screenPos, centerInScreen.z));
            auto point1 = mesh->addUVPoint(mesh->addVertex(pos), vec2(0));
            auto point2 = mesh->addUVPoint(mesh->addVertex(pos), vec2(0));
            _drawnUVPoints.push_back(point1);
            _drawnUVPoints.push_back(point2);
            mesh->addEdge({point1->vertex(), point2->vertex()});
        }
    }
}

void DrawTool::mouseMove(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target);

    auto mesh = item()->mesh();
    auto maybePrevVertex = lastDrawnVertex();
    if (maybePrevVertex) {
        auto prevVertex = *maybePrevVertex;

        auto [prevPosInScreen, isInScreen] = event.camera->mapWorldToScreen(prevVertex->position());
        if (!isInScreen) {
            return;
        }
        auto pos = event.camera->mapScreenToWorld(dvec3(event.screenPos, prevPosInScreen.z));
        mesh->setPositions({{prevVertex, pos}});
    }
}

void DrawTool::mouseRelease(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
}

void DrawTool::hoverEnter(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    // TOOD
}

void DrawTool::hoverLeave(const Tool::EventTarget &target) {
    Q_UNUSED(target);
    // TOOD
}

Opt<SP<Mesh::UVPoint> > DrawTool::lastDrawnPoint() const {
    if (_drawnUVPoints.empty()) {
        return {};
    }
    return _drawnUVPoints[_drawnUVPoints.size() - 1];
}

Opt<SP<Mesh::Vertex> > DrawTool::lastDrawnVertex() const {
    auto uvPoint = lastDrawnPoint();
    if (!uvPoint) {
        return {};
    }
    return (*uvPoint)->vertex();
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice