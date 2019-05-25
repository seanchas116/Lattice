#include "MeshEditor.hpp"
#include "MoveTool.hpp"
#include "DrawTool.hpp"
#include "ExtrudeTool.hpp"
#include "LoopCutTool.hpp"
#include "BorderSelectTool.hpp"
#include "../manipulator/MeshManipulator.hpp"
#include "../MeshVAOGenerator.hpp"
#include "../../state/AppState.hpp"
#include "../../state/MeshEditState.hpp"
#include "../../gl/VAO.hpp"
#include "../../gl/VertexBuffer.hpp"
#include "../../gl/Framebuffer.hpp"
#include "../../gl/Texture.hpp"
#include "../../gl/Binder.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../document/MeshObject.hpp"
#include "../../mesh/Mesh.hpp"
#include "../../support/Debug.hpp"
#include "../../support/Camera.hpp"
#include <QMouseEvent>
#include <QMenu>
#include <QPainter>
#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/algorithm/find.hpp>

using namespace glm;

namespace Lattice::Editor::MeshEditor {

namespace {

const vec4 unselectedColor = vec4(0, 0, 0, 1);
const vec4 selectedColor = vec4(1, 1, 1, 1);
const vec4 hoveredColor = vec4(1, 1, 0, 1);

const vec4 unselectedFaceColor = vec4(0, 0, 0, 1);
const vec4 selectedFaceColor = vec4(1, 1, 1, 1);
const vec4 hoveredFaceColor = vec4(1, 1, 0, 1);

const vec4 unselectedFaceHighlight = vec4(0, 0, 0, 0);
const vec4 selectedFaceHighlight = vec4(1, 1, 1, 0.5);
const vec4 hoveredFaceHighlight = vec4(1, 1, 0.5, 0.5);

glm::vec4 encodeIntToColor(int64_t value) {
    union {
        int64_t value;
        glm::u16vec4 color;
    } valueColor;
    valueColor.value = value;

    return glm::vec4(valueColor.color) / float(0xFFFF);
}

int64_t decodeColorToInt(glm::vec4 color) {
    union {
        int64_t value;
        glm::u16vec4 color;
    } valueColor;
    valueColor.color = glm::u16vec4(glm::round(color * float(0xFFFF)));
    return valueColor.value;
}

glm::vec4 encodeEventTargetToColor(const Tool::EventTarget& target) {
    int value;
    if (target.vertex) {
        value = target.vertex->index * 3;
    } else if (target.edge) {
        value = target.edge->index * 3 + 1;
    } else if (target.face) {
        value = target.face->index * 3 + 2;
    } else {
        value = -1;
    }
    return encodeIntToColor(value);
}

Tool::EventTarget decodeColorToEventTarget(glm::vec4 color) {
    int value = int(decodeColorToInt(color));
    if (value < 0) {
        return {};
    }
    switch (value % 3) {
    default:
    case 0:
        return {Mesh::VertexHandle(value / 3), {}, {}};
    case 1:
        return {{}, Mesh::EdgeHandle(value / 3), {}};
    case 2:
        return {{}, {}, Mesh::FaceHandle(value / 3)};
    }
}

}

MeshEditor::MeshEditor(const SP<State::AppState>& appState, const SP<State::MeshEditState> &meshEditState) :
    _appState(appState),
    _meshEditState(meshEditState),
    _manipulator(makeShared<Manipulator::MeshManipulator>(meshEditState->object()->location().matrixToWorld(), meshEditState->mesh())),

    _faceIBO(makeShared<GL::IndexBuffer>()),
    _faceVBO(makeShared<GL::VertexBuffer<Draw::Vertex>>()),
    _faceVAO(makeShared<GL::VAO>(_faceVBO, _faceIBO)),
    _facePickVBO(makeShared<GL::VertexBuffer<Draw::Vertex>>()),
    _facePickVAO(makeShared<GL::VAO>(_facePickVBO, _faceIBO)),
    _edgeVBO(makeShared<GL::VertexBuffer<Draw::PointLineVertex>>()),
    _edgeVAO(makeShared<GL::VAO>(_edgeVBO, GL::Primitive::Line)),
    _edgePickVBO(makeShared<GL::VertexBuffer<Draw::PointLineVertex>>()),
    _edgePickVAO(makeShared<GL::VAO>(_edgePickVBO, GL::Primitive::Line)),
    _vertexVBO(makeShared<GL::VertexBuffer<Draw::PointLineVertex>>()),
    _vertexVAO(makeShared<GL::VAO>(_vertexVBO, GL::Primitive::Point)),
    _vertexPickVBO(makeShared<GL::VertexBuffer<Draw::PointLineVertex>>()),
    _vertexPickVAO(makeShared<GL::VAO>(_vertexPickVBO, GL::Primitive::Point)),

    _tool(makeShared<MoveTool>(meshEditState)),

    _facesFramebuffer(makeShared<GL::Framebuffer>(glm::ivec2(0, 0))),
    _facesTexture(makeShared<GL::Texture>(glm::ivec2(0, 0))),
    _facesDepthTexture(makeShared<GL::Texture>(glm::ivec2(0, 0))),
    _hitFramebuffer(makeShared<GL::Framebuffer>(glm::ivec2(0, 0)))
{
    initializeOpenGLFunctions();
    updateVAOs();

    connect(appState.get(), &State::AppState::toolChanged, this, &MeshEditor::handleToolChange);
    handleToolChange(appState->tool());

    connect(_manipulator.get(), &Manipulator::Manipulator::onContextMenu, this, [this](auto& event) {
        contextMenuTarget({}, event);
    });
    connect(_manipulator.get(), &Manipulator::MeshManipulator::meshChanged, _meshEditState.get(), &State::MeshEditState::notifyMeshChanged);
    connect(_manipulator.get(), &Manipulator::MeshManipulator::meshChangeFinished, this, [this] {
        _meshEditState->commitMeshChanged(tr("Move Vertices"));
    });

    connect(appState.get(), &State::AppState::isTranslateHandleVisibleChanged, _manipulator.get(), &Manipulator::Manipulator::setTranslateHandleVisible);
    connect(appState.get(), &State::AppState::isRotateHandleVisibleChanged, _manipulator.get(), &Manipulator::Manipulator::setRotateHandleVisible);
    connect(appState.get(), &State::AppState::isScaleHandleVisibleChanged, _manipulator.get(), &Manipulator::Manipulator::setScaleHandleVisible);

    connect(meshEditState.get(), &State::MeshEditState::meshChanged, this, &MeshEditor::handleMeshChanged);
    connect(meshEditState->object().get(), &Document::MeshObject::subdivSettingsChanged, this, &MeshEditor::handleMeshChanged);
}

void MeshEditor::preDraw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    // TODO: use separate framebuffer for each viewport instead of resizing them
    updateVAOs();
    resizeFramebuffers(glm::ivec2(camera->viewportSize()));

    glEnable(GL_DEPTH_TEST);

    auto matrixToWorld = _meshEditState->object()->location().matrixToWorld();

    {
        GL::Binder binder(*_hitFramebuffer);
        operations->clear.clear(encodeEventTargetToColor({}), 1);
        if (_appState->isFaceSelectable()) {
            operations->drawUnicolor.draw(_facePickVAO, matrixToWorld, camera, vec4(0), true);
        }
        if (_appState->isEdgeSelectable()) {
            operations->drawLine.draw(_edgePickVAO, matrixToWorld, camera, 24.0, vec4(0), true, -0.0001);
        }
        if (_appState->isVertexSelectable()) {
            operations->drawCircle.draw(_vertexPickVAO, matrixToWorld, camera, 24.0, vec4(0), true, -0.0002);
        }
    }

    {
        // TODO: Use multisampled framebuffer
        GL::Binder binder(*_facesFramebuffer);
        operations->clear.clear(glm::vec4(0), 1);
        operations->drawUnicolor.draw(_faceVAO, matrixToWorld, camera, vec4(0), true);
    }
}

void MeshEditor::draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    auto matrixToWorld = _meshEditState->object()->location().matrixToWorld();
    auto& materials = _meshEditState->object()->materials();

    for (auto& [materialID, vao] : _finalShapeVAOs) {
        auto material = materials.at(materialID.index).toDrawMaterial();
        operations->drawMaterial.draw(vao, matrixToWorld, camera, material);
    }

    operations->drawLine.draw(_edgeVAO, matrixToWorld, camera, 1.0, vec4(0), true);
    if (_appState->isVertexSelectable()) {
        operations->drawCircle.draw(_vertexVAO, matrixToWorld, camera, 6.0, vec4(0), true);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    operations->copy.copy(_facesTexture, _facesDepthTexture, 0.4);
    glDisable(GL_BLEND);
}

void MeshEditor::drawHitArea(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    auto idColor = toIDColor();
    operations->clear.clear(idColor, 1);

    auto matrixToWorld = _meshEditState->object()->location().matrixToWorld();

    if (_appState->isFaceSelectable()) {
        operations->drawUnicolor.draw(_facePickVAO, matrixToWorld, camera, idColor, false);
    }
    if (_appState->isEdgeSelectable()) {
        operations->drawLine.draw(_edgePickVAO, matrixToWorld, camera, 12.0, idColor, false);
    }
    if (_appState->isVertexSelectable()) {
        operations->drawCircle.draw(_vertexPickVAO, matrixToWorld, camera, 24.0, idColor, false);
    }
}

void MeshEditor::mousePressEvent(const Viewport::MouseEvent &event) {
    mousePressTarget(pickEventTarget(event.viewportPos), event);
}

void MeshEditor::mouseMoveEvent(const Viewport::MouseEvent &event) {
    mouseMoveTarget(pickEventTarget(event.viewportPos), event);
}

void MeshEditor::mouseReleaseEvent(const Viewport::MouseEvent &event) {
    mouseReleaseTarget(pickEventTarget(event.viewportPos), event);
}

void MeshEditor::contextMenuEvent(const Viewport::MouseEvent &event) {
    contextMenuTarget(pickEventTarget(event.viewportPos), event);
}

void MeshEditor::keyPressEvent(QKeyEvent *event) {
    _tool->keyPressTool(event);
}

void MeshEditor::keyReleaseEvent(QKeyEvent *event) {
    _tool->keyReleaseTool(event);
}

void MeshEditor::handleToolChange(State::Tool tool) {
    switch (tool) {
    case State::Tool::Draw:
        _tool = makeShared<DrawTool>(_meshEditState);
        break;
    case State::Tool::Extrude:
        _tool = makeShared<ExtrudeTool>(_meshEditState);
        break;
    case State::Tool::LoopCut:
        _tool = makeShared<LoopCutTool>(_meshEditState);
        break;
    case State::Tool::BorderSelect:
        _tool = makeShared<BorderSelectTool>(_meshEditState);
        break;
    default:
        _tool = makeShared<MoveTool>(_meshEditState);
        break;
    }
    connect(_tool.get(), &Tool::finished, this, [this] {
        _appState->setTool(State::Tool::None);
    });
    updateManipulatorVisibility();
    updateChildren();
}

Tool::EventTarget MeshEditor::pickEventTarget(vec2 pos) {
    recallContext();
    PixelData<vec4> pixels(glm::ivec2(1));
    _hitFramebuffer->readPixels(pos, pixels);
    return decodeColorToEventTarget(pixels.data()[0]);
}

void MeshEditor::updateManipulatorVisibility() {
    auto& mesh = *_meshEditState->mesh();
    bool isVertexSelected = ranges::any_of(mesh.vertices(), [&] (auto v) { return mesh.isSelected(v); });
    _manipulator->setVisible(isVertexSelected  && _appState->tool() == State::Tool::None);
}

void MeshEditor::updateChildren() {
    setChildRenderables({_manipulator, _tool});
}

void MeshEditor::resizeFramebuffers(ivec2 size) {
    if (_framebufferSize != size) {
        _facesTexture = makeShared<GL::Texture>(size, GL::Texture::Format::RGBA8);
        _facesDepthTexture = makeShared<GL::Texture>(size, GL::Texture::Format::Depth24Stencil8);
        _facesFramebuffer = makeShared<GL::Framebuffer>(size, std::vector{_facesTexture}, _facesDepthTexture);

        _hitFramebuffer = makeShared<GL::Framebuffer>(size,
                                                      std::vector{makeShared<GL::Texture>(size, GL::Texture::Format::RGBA32F)},
                                                      makeShared<GL::Texture>(size, GL::Texture::Format::Depth24Stencil8));
        _framebufferSize = size;
    }
}

void MeshEditor::handleMeshChanged() {
    updateManipulatorVisibility();
    _manipulator->updatePosition();
    _isVAOsDirty = true;
    emit updated();
}

void MeshEditor::mousePressTarget(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    _tool->mousePressTool(target, event);
}

void MeshEditor::mouseMoveTarget(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    if (_hoveredTarget != target) {
        hoverLeaveTarget(_hoveredTarget);
        hoverEnterTarget(target, event);
    }
    _tool->mouseMoveTool(target, event);
}

void MeshEditor::mouseReleaseTarget(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    _tool->mouseReleaseTool(target, event);
}

void MeshEditor::hoverEnterTarget(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    _hoveredTarget = target;
    _isVAOsDirty = true;
    emit updated();

    _tool->hoverEnterTool(target, event);
}

void MeshEditor::hoverLeaveTarget(const Tool::EventTarget &target) {
    _hoveredTarget = {};
    _isVAOsDirty = true;
    emit updated();

    _tool->hoverLeaveTool(target);
}

void MeshEditor::contextMenuTarget(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target);

    QMenu contextMenu;
    contextMenu.addAction(tr("Delete Vertices"), _meshEditState.get(), &State::MeshEditState::deleteVertices);
    contextMenu.addAction(tr("Delete Edges"), _meshEditState.get(), &State::MeshEditState::deleteEdges);
    contextMenu.addAction(tr("Delete Faces"), _meshEditState.get(), &State::MeshEditState::deleteFaces);
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Select All"), _meshEditState.get(), &State::MeshEditState::selectAll);
    contextMenu.addAction(tr("Select Loop"), this, [this, target] {
        if (target.edge) {
            _meshEditState->loopSelect(*target.edge);
        }
    });
    contextMenu.addAction(tr("Select Belt"), this, [this, target] {
        if (target.edge) {
            _meshEditState->beltSelect(*target.edge);
        }
    });
    contextMenu.addAction(tr("Invert Selection"), _meshEditState.get(), &State::MeshEditState::invertSelection);
    contextMenu.addAction(tr("Deselect All"), _meshEditState.get(), &State::MeshEditState::deselectAll);
    contextMenu.exec(event.originalContextMenuEvent->globalPos());
}

void MeshEditor::updateVAOs() {
    if (!_isVAOsDirty) {
        return;
    }

    auto& mesh = *_meshEditState->mesh();
    auto hitTestExclusion = _tool->hitTestExclusion();

    auto& object = _meshEditState->object();
    MeshVAOGenerator generator(mesh);
    if (object->subdivSettings().isEnabled) {
        _finalShapeVAOs = generator.generateSubdivFaceVAOs(object->subdivSettings().segmentCount);
    } else {
        _finalShapeVAOs = generator.generateFaceVAOs();
    }

    {
        std::vector<Draw::PointLineVertex> vertexAttributes;
        std::vector<Draw::PointLineVertex> vertexPickAttributes;

        vertexAttributes.reserve(mesh.vertexCount());
        vertexPickAttributes.reserve(mesh.vertexCount());

        for (auto v : mesh.vertices()) {
            bool selected = mesh.isSelected(v);
            bool hovered = v == _hoveredTarget.vertex;
            bool hitTestExcluded = ranges::find(hitTestExclusion.vertices, v) != hitTestExclusion.vertices.end();

            Draw::PointLineVertex attrib;
            attrib.position = mesh.position(v);
            attrib.color = hovered ? hoveredColor : selected ? selectedColor : unselectedColor;
            vertexAttributes.push_back(attrib);

            Draw::PointLineVertex pickAttrib;
            pickAttrib.position = mesh.position(v);
            pickAttrib.color = encodeEventTargetToColor({v, {}, {}});
            pickAttrib.width = hitTestExcluded ? 0 : 1;
            vertexPickAttributes.push_back(pickAttrib);
        }

        _vertexVBO->setVertices(vertexAttributes);
        _vertexPickVBO->setVertices(vertexPickAttributes);
    }

    {
        std::vector<Draw::PointLineVertex> edgeAttributes;
        std::vector<Draw::PointLineVertex> edgePickAttributes;

        edgeAttributes.reserve(mesh.edgeCount() * 2);
        edgePickAttributes.reserve(mesh.edgeCount() * 2);

        for (auto e : mesh.edges()) {
            bool hovered = e == _hoveredTarget.edge;
            bool hitTestExcluded = ranges::find(hitTestExclusion.edges, e) != hitTestExclusion.edges.end();
            float crease = mesh.crease(e);
            vec4 indexColor = encodeEventTargetToColor({{}, e, {}});

            for (size_t vertexInEdgeIndex = 0; vertexInEdgeIndex < 2; ++vertexInEdgeIndex) {
                auto& v = mesh.vertices(e)[vertexInEdgeIndex];
                bool selected = mesh.isSelected(v);

                Draw::PointLineVertex attrib;
                attrib.position = mesh.position(v);
                attrib.color = hovered ? hoveredColor : selected ? selectedColor : unselectedColor;
                attrib.width = 1.f + crease / 10.f;
                edgeAttributes.push_back(attrib);

                Draw::PointLineVertex pickAttrib;
                pickAttrib.position = mesh.position(v);
                pickAttrib.color = indexColor;
                pickAttrib.width = hitTestExcluded ? 0 : 1;
                edgePickAttributes.push_back(pickAttrib);
            }
        }

        _edgeVBO->setVertices(edgeAttributes);
        _edgePickVBO->setVertices(edgePickAttributes);
    }

    {
        std::vector<Draw::Vertex> faceAttributes;
        std::vector<Draw::Vertex> facePickAttributes;
        std::vector<GL::IndexBuffer::Triangle> faceTriangles;
        faceAttributes.reserve(_facePickVBO->size());
        facePickAttributes.reserve(_facePickVBO->size());
        faceTriangles.reserve(_faceIBO->size() / 3);

        for (auto f : mesh.faces()) {
            auto i0 = uint32_t(facePickAttributes.size());
            for (uint32_t i = 2; i < uint32_t(mesh.vertices(f).size()); ++i) {
                auto i1 = i0 + i - 1;
                auto i2 = i0 + i;
                faceTriangles.push_back({i0, i1, i2});
            }

            bool selected = true;
            for (auto v : mesh.vertices(f)) {
                if (!mesh.isSelected(v)) {
                    selected = false;
                }
            }

            bool hovered = f == _hoveredTarget.face;
            vec4 indexColor = encodeEventTargetToColor({{}, {}, f});
            for (auto& p : mesh.uvPoints(f)) {
                auto position = mesh.position(mesh.vertex(p));

                Draw::Vertex attrib;
                attrib.position = position;
                attrib.color = hovered ? hoveredFaceColor : selected ? selectedFaceColor : unselectedFaceColor;
                faceAttributes.push_back(attrib);

                Draw::Vertex pickAttrib;
                pickAttrib.position = position;
                pickAttrib.color = indexColor;
                facePickAttributes.push_back(pickAttrib);
            }
        }
        _faceVBO->setVertices(faceAttributes);
        _facePickVBO->setVertices(facePickAttributes);
        _faceIBO->setTriangles(faceTriangles);
    }

    _isVAOsDirty = false;

    updateChildren();
}

}
