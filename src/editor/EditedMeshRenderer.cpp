#include "EditedMeshRenderer.hpp"
#include "MeshPicker.hpp"
#include "../ui/AppState.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/PointVAO.hpp"
#include "../gl/VAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../document/Mesh.hpp"
#include "../document/MeshItem.hpp"
#include "../support/Debug.hpp"
#include "../support/Camera.hpp"
#include <QMouseEvent>

using namespace glm;

namespace Lattice::Editor {

namespace {

const vec3 unselectedColor = vec3(0);
const vec3 selectedColor = vec3(1);

}

EditedMeshRenderer::EditedMeshRenderer(const SP<UI::AppState>& appState, const SP<Document::MeshItem> &item) :
    _appState(appState),
    _item(item),
    _meshPicker(makeShared<MeshPicker>(item->mesh())),
    _faceVBO(makeShared<GL::VertexBuffer<GL::Vertex>>()),
    _edgeVAO(makeShared<GL::LineVAO>()),
    _vertexVAO(makeShared<GL::PointVAO>())
{
    updateWholeVAOs();
    connect(_item->mesh().get(), &Document::Mesh::changed, this, &EditedMeshRenderer::updateWholeVAOs);
    connect(_appState->document().get(), &Document::Document::meshSelectionChanged, this, &EditedMeshRenderer::updateWholeVAOs);
}

void EditedMeshRenderer::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    if (_appState->isFaceVisible()) {
        for (auto& [material, vao] : _faceVAOs) {
            operations->drawMaterial.draw(vao, _item->location().matrixToWorld(), camera, material);
        }
    }
    if (_appState->isEdgeVisible()) {
        operations->drawLine.draw(_edgeVAO, _item->location().matrixToWorld(), camera, 1.0, dvec3(0), true);
    }
    if (_appState->isVertexVisible()) {
        operations->drawCircle.draw(_vertexVAO, _item->location().matrixToWorld(), camera, 6.0, dvec3(0), true);
    }
}

std::optional<Render::HitResult> EditedMeshRenderer::hitTest(dvec2 pos, const SP<Camera> &camera) const {
    auto vertexPickResult = _meshPicker->pickVertex(_item->location().matrixToWorld(), camera, pos, 12);
    if (vertexPickResult) {
        auto [vertex, depth] = *vertexPickResult;
        Render::HitResult result;
        result.depth = depth;
        result.vertex = vertex;
        return result;
    }
    auto edgePickResult = _meshPicker->pickEdge(_item->location().matrixToWorld(), camera, pos, 6);
    if (edgePickResult) {
        auto [edge, depth] = *edgePickResult;
        Render::HitResult result;
        result.depth = depth;
        result.edge = edge;
        return result;
    }
    auto facePickResult = _meshPicker->pickFace(_item->location().matrixToWorld(), camera, pos);
    if (facePickResult) {
        auto [face, depth] = *facePickResult;
        Render::HitResult result;
        result.depth = depth;
        result.face = face;
        return result;
    }

    return std::nullopt;
}

void EditedMeshRenderer::mousePress(const Render::MouseEvent &event) {
    std::unordered_set<SP<Document::MeshVertex>> newVertices;
    if (event.hitResult.vertex) {
        newVertices.insert(*event.hitResult.vertex);
    } else if (event.hitResult.edge) {
        auto& edge = *event.hitResult.edge;
        newVertices.insert(edge->vertices()[0]);
        newVertices.insert(edge->vertices()[1]);
    } else if (event.hitResult.face) {
        auto& face = *event.hitResult.face;
        for (auto& v : face->vertices()) {
            newVertices.insert(v);
        }
    }

    Document::MeshSelection selection;
    if (event.originalEvent->modifiers() & Qt::ShiftModifier) {
        selection = _appState->document()->meshSelection();

        bool alreadyAdded = true;
        for (auto& v : newVertices) {
            if (selection.vertices.find(v) == selection.vertices.end()) {
                alreadyAdded = false;
            }
        }
        if (alreadyAdded) {
            for (auto& v : newVertices) {
                selection.vertices.erase(v);
            }
        } else {
            for (auto& v: newVertices) {
                selection.vertices.insert(v);
            }
        }
    } else {
        selection.vertices = newVertices;
    }

    _initialPositions.clear();
    for (auto& v : selection.vertices) {
        _initialPositions[v] = v->position();
    }
    _dragStartWorldPos = event.camera->mapScreenToWorld(glm::vec3(event.screenPos, event.hitResult.depth));
    _dragStarted = false;

    _appState->document()->setMeshSelection(selection);
}

void EditedMeshRenderer::mouseMove(const Render::MouseEvent &event) {
    dvec3 worldPos = event.camera->mapScreenToWorld(glm::vec3(event.screenPos, event.hitResult.depth));
    dvec3 offset = worldPos - _dragStartWorldPos;

    if (!_dragStarted) {
        _appState->document()->history()->beginChange(tr("Move Vertex"));
        _dragStarted = true;
    }

    auto& mesh = _item->mesh();
    std::unordered_map<SP<Document::MeshVertex>, vec3> positions;
    for (auto& [v, initialPos] : _initialPositions) {
        positions[v] = initialPos + offset;
    }
    mesh->setPositions(positions);
}

void EditedMeshRenderer::mouseRelease(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void EditedMeshRenderer::updateWholeVAOs() {
    recallContext();

    auto& selectedVertices = _appState->document()->meshSelection().vertices;

    {
        _vertexVAO = makeShared<GL::PointVAO>();
        _vertexAttributes.clear();
        _vertexAttributes.reserve(_item->mesh()->vertices().size());

        for (auto& v : _item->mesh()->vertices()) {
            bool selected = selectedVertices.find(v) != selectedVertices.end();

            GL::Vertex attrib;
            attrib.position = v->position();
            attrib.color = selected ? selectedColor : unselectedColor;

            _vertexAttributes.push_back(attrib);
        }

        _vertexVAO->vertexBuffer()->setVertices(_vertexAttributes);
    }

    {
        _edgeVAO = makeShared<GL::LineVAO>();
        _edgeAttributes.clear();
        _edgeAttributes.reserve(_item->mesh()->edges().size() * 2);

        std::vector<GL::LineVAO::Line> indices;
        for (auto& [_, e] : _item->mesh()->edges()) {
            auto offset = uint32_t(_edgeAttributes.size());
            for (auto& v : e->vertices()) {
                bool selected = selectedVertices.find(v) != selectedVertices.end();

                GL::Vertex attrib;
                attrib.position = v->position();
                attrib.color = selected ? selectedColor : unselectedColor;
                _edgeAttributes.push_back(attrib);
            }
            indices.push_back({offset, offset+1});
        }

        _edgeVAO->vertexBuffer()->setVertices(_edgeAttributes);
        _edgeVAO->setLines(indices);
    }

    {
        _faceVBO = makeShared<GL::VertexBuffer<GL::Vertex>>();

        _faceVAOs.clear();
        _faceAttributes.clear();

        auto addPoint = [&](const SP<Document::MeshUVPoint>& p) {
            GL::Vertex attrib;
            attrib.position = p->vertex()->position();
            attrib.texCoord = p->position();
            attrib.normal = p->vertex()->normal();

            auto index = uint32_t(_faceAttributes.size());
            _faceAttributes.push_back(attrib);
            return index;
        };

        for (auto& material : _item->mesh()->materials()) {
            auto vao = makeShared<GL::OldVAO>(_faceVBO);
            std::vector<GL::OldVAO::Triangle> triangles;
            for (auto& face : material->faces()) {
                auto i0 = addPoint(face->uvPoints()[0]);
                for (uint32_t i = 2; i < uint32_t(face->vertices().size()); ++i) {
                    auto i1 = addPoint(face->uvPoints()[i - 1]);
                    auto i2 = addPoint(face->uvPoints()[i]);
                    triangles.push_back({i0, i1, i2});
                }
            }
            vao->setTriangles(triangles);
            _faceVAOs.insert({material, vao});
        }
        _faceVBO->setVertices(_faceAttributes);
    }

    updateRequested();
}

}
