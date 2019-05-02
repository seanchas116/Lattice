#include "DrawTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../support/Distance.hpp"
#include "../../support/Debug.hpp"
#include "../../mesh/algorithm/CutEdge.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

Tool::HitTestExclusion DrawTool::hitTestExclusion() const {
    if (!_previewUVPoint) {
        return {};
    }
    auto uvPoint = *_previewUVPoint;
    auto& mesh = *this->mesh();
    std::vector<Mesh::EdgeHandle> edges = mesh.edges(mesh.vertex(uvPoint)) | ranges::to_vector;
    return {{mesh.vertex(uvPoint)}, edges, {}};
}

void DrawTool::mousePressTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    auto& mesh = *this->mesh();
    auto modelMatrix = object()->location().matrixToWorld();

    if (_drawnUVPoints.empty()) {
        appState()->document()->history()->beginChange(tr("Draw"));
    }

    if (target.vertex) {
        if (_previewUVPoint) {
            mesh.removeVertex(mesh.vertex(*_previewUVPoint));
        }

        auto vertex = *target.vertex;
        auto uvPoint = mesh.uvPoints(vertex).front();
        auto closingPointIt = ranges::find(_drawnUVPoints, uvPoint);
        if (closingPointIt != _drawnUVPoints.end()) {
            // create face
            std::vector<Mesh::UVPointHandle> points(closingPointIt, _drawnUVPoints.end());
            auto face = mesh.addFace(points, 0); // TODO: use better material

            Q_UNUSED(face);
            // TODO: flip face (or use two-sided faces)
            /*
            bool isFaceFore = dot(face->normal(), event.camera->location().backward()) > 0;
            if (!isFaceFore) {
                //mesh->flipFace(face);
            }
            */

            _drawnUVPoints.clear();
            _previewUVPoint = std::nullopt;

            OldMesh::MeshFragment selection;
            mesh.clearSelections();
            for (auto& p : points) {
                mesh.setSelected(mesh.vertex(p), true);
            }
            emit finished();

            return;
        }

        if (_drawnUVPoints.size() >= 1) {
            auto prevUVPoint = _drawnUVPoints[_drawnUVPoints.size() - 1];
            mesh.addEdge(mesh.vertex(prevUVPoint), vertex, false);
        }
        _drawnUVPoints.push_back(uvPoint);
    } else if (target.edge) {
        if (_previewUVPoint) {
            mesh.removeVertex(mesh.vertex(*_previewUVPoint));
        }

        auto edge = *target.edge;
        auto pos0 = mesh.position(mesh.vertices(edge)[0]);
        auto pos1 = mesh.position(mesh.vertices(edge)[1]);
        Ray<double> edgeRay(pos0, pos1 - pos0);
        Ray<double> mouseRay = event.camera->modelMouseRay(modelMatrix, event.viewportPos);
        RayRayDistanceSolver distanceSolver(edgeRay, mouseRay);
        auto vertex = Mesh::CutEdge(edge, distanceSolver.t0).redo(mesh);

        if (_drawnUVPoints.size() >= 1) {
            auto prevUVPoint = _drawnUVPoints[_drawnUVPoints.size() - 1];
            mesh.addEdge(mesh.vertex(prevUVPoint), vertex, false);
        }

        _drawnUVPoints.push_back(mesh.uvPoints(vertex).front());
    } else {
        if (_previewUVPoint) {
            // confirm preview point

            auto previewUVPoint = *_previewUVPoint;
            _previewUVPoint = std::nullopt;
            auto [prevPosInViewport, isInViewport] = event.camera->mapModelToViewport(modelMatrix, mesh.position(mesh.vertex(previewUVPoint)));
            if (!isInViewport) {
                return;
            }
            auto pos = event.camera->mapViewportToModel(modelMatrix, dvec3(event.viewportPos.xy, prevPosInViewport.z));

            mesh.setPosition(mesh.vertex(previewUVPoint), pos);
            _drawnUVPoints.push_back(previewUVPoint);
        } else {
            // add new point
            auto [centerInViewport, isCenterInViewport] = event.camera->mapModelToViewport(modelMatrix, vec3(0));
            if (!isCenterInViewport) {
                return;
            }
            auto pos = event.camera->mapViewportToModel(modelMatrix, dvec3(event.viewportPos.xy, centerInViewport.z));
            _drawnUVPoints.push_back(mesh.addUVPoint(mesh.addVertex(pos), vec2(0)));
        }
    }

    auto latestPoint = _drawnUVPoints[_drawnUVPoints.size() - 1];
    auto previewPoint = mesh.addUVPoint(mesh.addVertex(mesh.position(mesh.vertex(latestPoint))), vec2(0));
    mesh.addEdge(mesh.vertex(latestPoint), mesh.vertex(previewPoint), false);
    _previewUVPoint = previewPoint;
}

void DrawTool::mouseMoveTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target);

    auto& mesh = *this->mesh();
    auto modelMatrix = object()->location().matrixToWorld();

    if (!_previewUVPoint) {
        return;
    }
    auto previewUVPoint = *_previewUVPoint;

    vec3 pos{0};

    if (target.vertex) {
        auto snapVertex = *target.vertex;
        pos = mesh.position(snapVertex);
    } else if (target.edge) {
        auto snapEdge = *target.edge;
        Ray<double> edgeRay = snapEdge->ray();
        Ray<double> mouseRay = event.camera->modelMouseRay(modelMatrix, event.viewportPos);
        RayRayDistanceSolver distanceSolver(edgeRay, mouseRay);
        pos = edgeRay.at(distanceSolver.t0);
    } else {
        auto [prevPosInViewport, isInViewport] = event.camera->mapModelToViewport(modelMatrix, previewUVPoint->vertex()->position());
        if (!isInViewport) {
            return;
        }
        pos = event.camera->mapViewportToModel(modelMatrix, dvec3(event.viewportPos.xy, prevPosInViewport.z));
    }

    mesh->setPosition({{previewUVPoint->vertex(), pos}});
}

void DrawTool::mouseReleaseTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
}

void DrawTool::keyPressTool(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        if (_previewUVPoint) {
            object()->oldMesh()->removeVertex((*_previewUVPoint)->vertex());
        }
        _drawnUVPoints.clear();
        _previewUVPoint = std::nullopt;
        emit finished();
    }
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
