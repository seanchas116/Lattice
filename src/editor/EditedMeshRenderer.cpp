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
    _meshPicker(makeShared<MeshPicker>(item->location().matrix(), item->mesh())),
    _faceVAOs(generateFaceVAOs()),
    _edgeVAO(generateEdgeVAO()),
    _vertexVAO(generateVertexVAO())
{
    connect(_item.get(), &Document::MeshItem::meshChanged, this, &EditedMeshRenderer::updateVAOs);
    connect(_item.get(), &Document::Item::locationChanged, this, [this](auto& loc) {
        _meshPicker->setMatrix(loc.matrix());
    });
    connect(_appState->document().get(), &Document::Document::meshSelectionChanged, this, &EditedMeshRenderer::updateVAOs);
}

void EditedMeshRenderer::draw(const SP<Render::Operations> &operations, const Camera &camera) {
    if (_appState->isFaceVisible()) {
        for (auto& [material, vao] : _faceVAOs) {
            operations->drawMaterial.draw(vao, _item->location().matrix(), camera, material);
        }
    }
    if (_appState->isEdgeVisible()) {
        operations->drawLine.draw(_edgeVAO, _item->location().matrix(), camera, 1.0, dvec3(0), true);
    }
    if (_appState->isVertexVisible()) {
        operations->drawCircle.draw(_vertexVAO, _item->location().matrix(), camera, 6.0, dvec3(0), true);
    }
}

std::optional<Render::HitResult> EditedMeshRenderer::hitTest(dvec2 pos, const Camera &camera) const {
    auto worldMouseRay = camera.worldMouseRay(pos);
    auto modelMouseRay = glm::inverse(_item->location().matrix()) * worldMouseRay;
    auto pickResult = _meshPicker->pickVertex(modelMouseRay, 0.1); // TODO: tweak hit distance based on depth
    if (!pickResult) {
        return std::nullopt;
    }
    auto [vertex, t] = *pickResult;
    Render::HitResult result;
    result.t = t;
    result.vertex = vertex;
    return result;
}

void EditedMeshRenderer::mousePress(const Render::MouseEvent &event) {
    if (event.hitResult.vertex) {
        Document::MeshSelection selection;
        if (event.originalEvent->modifiers() & Qt::ShiftModifier) {
            selection = _appState->document()->meshSelection();
        }
        selection.vertices.insert(*event.hitResult.vertex);
        _appState->document()->setMeshSelection(selection);
    }
}

void EditedMeshRenderer::mouseMove(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void EditedMeshRenderer::mouseRelease(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void EditedMeshRenderer::updateVAOs() {
    recallContext();
    _vertexVAO = generateVertexVAO();
    _edgeVAO = generateEdgeVAO();
    _faceVAOs = generateFaceVAOs();
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
