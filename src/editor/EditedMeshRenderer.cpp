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
    _faceVAOs(generateFaceVAOs()),
    _edgeVAO(generateEdgeVAO()),
    _vertexVAO(generateVertexVAO())
{
    connect(_item.get(), &Document::MeshItem::meshChanged, this, &EditedMeshRenderer::updateVAOs);
    connect(_appState->document().get(), &Document::Document::meshSelectionChanged, this, &EditedMeshRenderer::updateVAOs);
}

void EditedMeshRenderer::draw(const SP<Render::Operations> &operations, const Camera &camera) {
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

std::optional<Render::HitResult> EditedMeshRenderer::hitTest(dvec2 pos, const Camera &camera) const {
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
    _dragStartWorldPos = event.camera.mapScreenToWorld(glm::vec3(event.screenPos, event.hitResult.depth));
    qDebug() << _dragStartWorldPos;

    _appState->document()->setMeshSelection(selection);
}

void EditedMeshRenderer::mouseMove(const Render::MouseEvent &event) {
    dvec3 worldPos = event.camera.mapScreenToWorld(glm::vec3(event.screenPos, event.hitResult.depth));
    dvec3 offset = worldPos - _dragStartWorldPos;

    for (auto& [v, initialPos] : _initialPositions) {
        v->setPosition(initialPos + offset);
    }
    _item->emitMeshChanged();
}

void EditedMeshRenderer::mouseRelease(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void EditedMeshRenderer::updateVAOs() {
    recallContext();
    _vertexVAO = generateVertexVAO();
    _edgeVAO = generateEdgeVAO();
    _faceVAOs = generateFaceVAOs();

    updateRequested();
}

SP<GL::PointVAO> EditedMeshRenderer::generateVertexVAO() const {
    auto& selectedVertices = _appState->document()->meshSelection().vertices;

    auto vao = makeShared<GL::PointVAO>();
    std::vector<GL::VertexBuffer::Vertex> attribs;
    for (auto& v : _item->mesh()->vertices()) {
        bool selected = selectedVertices.find(v) != selectedVertices.end();

        GL::VertexBuffer::Vertex attrib;
        attrib.position = v->position();
        attrib.color = selected ? selectedColor : unselectedColor;

        attribs.push_back(attrib);
    }

    vao->vertexBuffer()->setVertices(attribs);
    return vao;
}

SP<GL::LineVAO> EditedMeshRenderer::generateEdgeVAO() const {
    auto& selectedVertices = _appState->document()->meshSelection().vertices;

    auto vao = makeShared<GL::LineVAO>();
    std::vector<GL::VertexBuffer::Vertex> attribs;
    std::vector<GL::LineVAO::Line> indices;
    for (auto& [_, e] : _item->mesh()->edges()) {
        auto offset = uint32_t(attribs.size());
        for (auto& v : e->vertices()) {
            bool selected = selectedVertices.find(v) != selectedVertices.end();

            GL::VertexBuffer::Vertex attrib;
            attrib.position = v->position();
            attrib.color = selected ? selectedColor : unselectedColor;
            attribs.push_back(attrib);
        }
        indices.push_back({offset, offset+1});
    }

    vao->vertexBuffer()->setVertices(attribs);
    vao->setLines(indices);
    return vao;
}

std::unordered_map<SP<Document::MeshMaterial>, SP<GL::VAO> > EditedMeshRenderer::generateFaceVAOs() const {
    auto vbo = makeShared<GL::VertexBuffer>();
    std::unordered_map<SP<Document::MeshMaterial>, SP<GL::VAO> > faceVAOs;
    std::vector<GL::VertexBuffer::Vertex> attribs;

    auto addPoint = [&](const SP<Document::MeshUVPoint>& p) {
        GL::VertexBuffer::Vertex attrib;
        attrib.position = p->vertex()->position();
        attrib.texCoord = p->position();
        attrib.normal = p->vertex()->normal();

        auto index = uint32_t(attribs.size());
        attribs.push_back(attrib);
        return index;
    };

    for (auto& material : _item->mesh()->materials()) {
        auto vao = makeShared<GL::VAO>(vbo);
        std::vector<GL::VAO::Triangle> triangles;
        for (auto& face : material->faces()) {
            auto i0 = addPoint(face->uvPoints()[0]);
            for (uint32_t i = 2; i < uint32_t(face->vertices().size()); ++i) {
                auto i1 = addPoint(face->uvPoints()[i - 1]);
                auto i2 = addPoint(face->uvPoints()[i]);
                triangles.push_back({i0, i1, i2});
            }
        }
        vao->setTriangles(triangles);
        faceVAOs.insert({material, vao});
    }
    vbo->setVertices(attribs);

    return faceVAOs;
}

}
