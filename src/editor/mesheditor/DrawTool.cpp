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

            mesh.deselectAll();
            for (auto& p : points) {
                mesh.setSelected(mesh.vertex(p), true);
            }
            meshEditState()->commitMeshChanged(tr("Draw"));
            emit finished();

            return;
        }

        if (_drawnUVPoints.size() >= 1) {
            auto prevUVPoint = _drawnUVPoints[_drawnUVPoints.size() - 1];
            mesh.addEdge(mesh.vertex(prevUVPoint), vertex);
        }
        _drawnUVPoints.push_back(uvPoint);
    } else if (target.edge) {
        if (_previewUVPoint) {
            mesh.removeVertex(mesh.vertex(*_previewUVPoint));
        }

        auto edge = *target.edge;
        Ray<double> edgeRay = mesh.ray(edge);
        Ray<double> mouseRay = event.camera->modelMouseRay(modelMatrix, event.viewportPos);
        RayRayDistanceSolver distanceSolver(edgeRay, mouseRay);
        auto vertex = Mesh::CutEdge(edge, distanceSolver.t0).perform(mesh);

        if (_drawnUVPoints.size() >= 1) {
            auto prevUVPoint = _drawnUVPoints[_drawnUVPoints.size() - 1];
            mesh.addEdge(mesh.vertex(prevUVPoint), vertex);
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
    mesh.addEdge(mesh.vertex(latestPoint), mesh.vertex(previewPoint));
    _previewUVPoint = previewPoint;

    meshEditState()->notifyMeshChanged();
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
        Ray<double> edgeRay = mesh.ray(snapEdge);
        Ray<double> mouseRay = event.camera->modelMouseRay(modelMatrix, event.viewportPos);
        RayRayDistanceSolver distanceSolver(edgeRay, mouseRay);
        pos = edgeRay.at(distanceSolver.t0);
    } else {
        auto [prevPosInViewport, isInViewport] = event.camera->mapModelToViewport(modelMatrix, mesh.position(mesh.vertex(previewUVPoint)));
        if (!isInViewport) {
            return;
        }
        pos = event.camera->mapViewportToModel(modelMatrix, dvec3(event.viewportPos.xy, prevPosInViewport.z));
    }

    mesh.setPosition(mesh.vertex(previewUVPoint), pos);

    meshEditState()->notifyMeshChanged();
}

void DrawTool::mouseReleaseTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
}

void DrawTool::keyPressTool(QKeyEvent *event) {
    auto& mesh = *this->mesh();

    if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        if (_previewUVPoint) {
            mesh.removeVertex(mesh.vertex(*_previewUVPoint));
        }
        _drawnUVPoints.clear();
        _previewUVPoint = std::nullopt;
        meshEditState()->commitMeshChanged(tr("Draw"));
        emit finished();
    }
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
