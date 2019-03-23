#include "DrawTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../support/Distance.hpp"
#include "../../support/Debug.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

Tool::HitTestExclusion DrawTool::hitTestExclusion() const {
    if (!_previewUVPoint) {
        return {};
    }
    auto uvPoint = *_previewUVPoint;
    std::vector<SP<Mesh::Edge>> edges;
    for (auto& e : uvPoint->vertex()->edges()) {
        edges.push_back(e->sharedFromThis());
    }

    return {{uvPoint->vertex()}, edges, {}};
}

void DrawTool::mousePress(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    auto mesh = item()->mesh();
    auto modelMatrix = item()->location().matrixToWorld();

    if (_drawnUVPoints.empty()) {
        appState()->document()->history()->beginChange(tr("Draw"));
    }

    if (target.vertex) {
        if (_previewUVPoint) {
            mesh->removeVertex((*_previewUVPoint)->vertex());
        }

        auto vertex = *target.vertex;
        auto closingPointIt = std::find(_drawnUVPoints.begin(), _drawnUVPoints.end(), vertex->firstUVPoint());
        if (closingPointIt != _drawnUVPoints.end()) {
            // create face
            std::vector<SP<Mesh::UVPoint>> points(closingPointIt, _drawnUVPoints.end());
            auto face = mesh->addFace(points, mesh->materials()[0]);

            bool isFaceFore = dot(face->normal(), vec3(event.camera->location().backward())) > 0;
            if (!isFaceFore) {
                mesh->flipFace(face);
            }
            _drawnUVPoints.clear();
            _previewUVPoint = std::nullopt;

            Mesh::MeshFragment selection;
            for (auto& p : points) {
                selection.vertices.insert(p->vertex());
            }
            appState()->document()->setMeshSelection(selection);

            return;
        }

        if (_drawnUVPoints.size() >= 1) {
            auto prevUVPoint = _drawnUVPoints[_drawnUVPoints.size() - 1];
            mesh->addEdge({prevUVPoint->vertex(), vertex});
        }
        _drawnUVPoints.push_back(vertex->firstUVPoint());
    } else if (target.edge) {
        if (_previewUVPoint) {
            mesh->removeVertex((*_previewUVPoint)->vertex());
        }

        auto edge = *target.edge;
        Ray<double> edgeRay = edge->ray();
        Ray<double> mouseRay = event.camera->modelMouseRay(modelMatrix, event.screenPos);
        RayRayDistanceSolver distanceSolver(edgeRay, mouseRay);
        auto vertex = mesh->cutEdge(edge, distanceSolver.t0);

        if (_drawnUVPoints.size() >= 1) {
            auto prevUVPoint = _drawnUVPoints[_drawnUVPoints.size() - 1];
            mesh->addEdge({prevUVPoint->vertex(), vertex});
        }

        _drawnUVPoints.push_back(vertex->firstUVPoint());
    } else {
        if (_previewUVPoint) {
            // confirm preview point

            auto previewUVPoint = *_previewUVPoint;
            _previewUVPoint = std::nullopt;
            auto [prevPosInScreen, isInScreen] = event.camera->mapModelToScreen(modelMatrix, previewUVPoint->vertex()->position());
            if (!isInScreen) {
                return;
            }
            auto pos = event.camera->mapScreenToModel(modelMatrix, dvec3(event.screenPos, prevPosInScreen.z));

            mesh->setPosition({{previewUVPoint->vertex(), pos}});
            _drawnUVPoints.push_back(previewUVPoint);
        } else {
            // add new point
            auto [centerInScreen, isCenterInScreen] = event.camera->mapModelToScreen(modelMatrix, vec3(0));
            if (!isCenterInScreen) {
                return;
            }
            auto pos = event.camera->mapScreenToModel(modelMatrix, dvec3(event.screenPos, centerInScreen.z));
            _drawnUVPoints.push_back(mesh->addUVPoint(mesh->addVertex(pos), vec2(0)));
        }
    }

    auto latestPoint = _drawnUVPoints[_drawnUVPoints.size() - 1];
    auto previewPoint = mesh->addUVPoint(mesh->addVertex(latestPoint->vertex()->position()), vec2(0));
    mesh->addEdge({latestPoint->vertex(), previewPoint->vertex()});
    _previewUVPoint = previewPoint;
}

void DrawTool::mouseMove(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target);

    auto mesh = item()->mesh();
    auto modelMatrix = item()->location().matrixToWorld();

    if (!_previewUVPoint) {
        return;
    }
    auto previewUVPoint = *_previewUVPoint;

    vec3 pos{0};

    if (target.vertex) {
        auto snapVertex = *target.vertex;
        pos = snapVertex->position();
    } else if (target.edge) {
        auto snapEdge = *target.edge;
        Ray<double> edgeRay = snapEdge->ray();
        Ray<double> mouseRay = event.camera->modelMouseRay(modelMatrix, event.screenPos);
        RayRayDistanceSolver distanceSolver(edgeRay, mouseRay);
        pos = edgeRay.at(distanceSolver.t0);
    } else {
        auto [prevPosInScreen, isInScreen] = event.camera->mapModelToScreen(modelMatrix, previewUVPoint->vertex()->position());
        if (!isInScreen) {
            return;
        }
        pos = event.camera->mapScreenToModel(modelMatrix, dvec3(event.screenPos, prevPosInScreen.z));
    }

    mesh->setPosition({{previewUVPoint->vertex(), pos}});
}

void DrawTool::mouseRelease(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
}

void DrawTool::keyPress(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        if (_previewUVPoint) {
            item()->mesh()->removeVertex((*_previewUVPoint)->vertex());
        }
        _drawnUVPoints.clear();
        _previewUVPoint = std::nullopt;
    }
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
