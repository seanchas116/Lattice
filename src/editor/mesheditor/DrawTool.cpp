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
    auto maybeVertex = lastDrawnVertex();
    if (!maybeVertex) {
        return {};
    }
    auto vertex = *maybeVertex;
    std::vector<SP<Mesh::Edge>> edges;
    for (auto& e : vertex->edges()) {
        edges.push_back(e->sharedFromThis());
    }

    return {{vertex}, edges, {}};
}

void DrawTool::mousePress(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    auto mesh = item()->mesh();
    auto modelMatrix = item()->location().matrixToWorld();

    if (!_drawnUVPoints.empty()) {
        if (target.vertex) {
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
            auto [prevPosInScreen, isInScreen] = event.camera->mapModelToScreen(modelMatrix, prevUVPoint->vertex()->position());
            if (!isInScreen) {
                return;
            }
            auto pos = event.camera->mapScreenToModel(modelMatrix, dvec3(event.screenPos, prevPosInScreen.z));

            mesh->setPositions({{prevUVPoint->vertex(), pos}});

            auto uvPoint = mesh->addUVPoint(mesh->addVertex(pos), vec2(0));
            _drawnUVPoints.push_back(uvPoint);
            mesh->addEdge({prevUVPoint->vertex(), uvPoint->vertex()});
        }
    } else {
        appState()->document()->history()->beginChange(tr("Draw"));

        if (target.vertex) {
            // start from existing vertex
            auto& vertex = *target.vertex;
            auto point1 = (*vertex->uvPoints().begin())->sharedFromThis();
            auto point2 = mesh->addUVPoint(mesh->addVertex(vertex->position()), vec2(0));
            _drawnUVPoints.push_back(point1);
            _drawnUVPoints.push_back(point2);
            mesh->addEdge({point1->vertex(), point2->vertex()});
        } else if (target.edge) {
            // start from edge
            auto edge = *target.edge;
            Ray<double> edgeRay = edge->ray();
            Ray<double> mouseRay = event.camera->modelMouseRay(modelMatrix, event.screenPos);
            RayRayDistanceSolver distanceSolver(edgeRay, mouseRay);
            auto pos = edgeRay.at(distanceSolver.t0);

            auto point1 = mesh->addUVPoint(mesh->addVertex(pos), vec2(0));
            auto point2 = mesh->addUVPoint(mesh->addVertex(pos), vec2(0));
            _drawnUVPoints.push_back(point1);
            _drawnUVPoints.push_back(point2);
            mesh->addEdge({point1->vertex(), point2->vertex()});

            auto edge1 = mesh->addEdge({edge->vertices()[0], point1->vertex()});
            auto edge2 = mesh->addEdge({point1->vertex(), edge->vertices()[1]});

            auto faces = edge->faces();
            for (auto& face : faces) {
                std::vector<SP<Mesh::UVPoint>> newFaceUVPoints;
                for (size_t i = 0; i < face->uvPoints().size(); ++i) {
                    auto uv0 = face->uvPoints()[i];
                    auto uv1 = face->uvPoints()[(i + 1) % face->uvPoints().size()];

                    newFaceUVPoints.push_back(uv0);

                    // TODO: create separate uvpoint if possible when uv is split at the edge
                    if (uv0->vertex() == edge->vertices()[0] && uv1->vertex() == edge->vertices()[1]) {
                        newFaceUVPoints.push_back(point1);
                    } else if (uv1->vertex() == edge->vertices()[0] && uv0->vertex() == edge->vertices()[1]) {
                        newFaceUVPoints.push_back(point1);
                    }
                }
                mesh->addFace(newFaceUVPoints, face->material());
            }
            for (auto& face : faces) {
                mesh->removeFace(face);
            }
            mesh->removeEdge(edge);
        } else {
            // start from new vertex
            // TODO: better depth
            auto [centerInScreen, isCenterInScreen] = event.camera->mapModelToScreen(modelMatrix, vec3(0));
            if (!isCenterInScreen) {
                return;
            }
            auto pos = event.camera->mapScreenToModel(modelMatrix, dvec3(event.screenPos, centerInScreen.z));
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
    auto modelMatrix = item()->location().matrixToWorld();
    auto maybePrevVertex = lastDrawnVertex();
    if (maybePrevVertex) {
        auto prevVertex = *maybePrevVertex;

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
            auto [prevPosInScreen, isInScreen] = event.camera->mapModelToScreen(modelMatrix, prevVertex->position());
            if (!isInScreen) {
                return;
            }
            pos = event.camera->mapScreenToModel(modelMatrix, dvec3(event.screenPos, prevPosInScreen.z));
        }

        mesh->setPositions({{prevVertex, pos}});
    }
}

void DrawTool::mouseRelease(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
}

void DrawTool::keyPress(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        // abort
        item()->mesh()->removeVertex(_drawnUVPoints[_drawnUVPoints.size() - 1]->vertex());
        _drawnUVPoints.clear();
    } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        // finish
        _drawnUVPoints.clear();
    }
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
