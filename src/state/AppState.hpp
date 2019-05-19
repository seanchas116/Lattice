#pragma once

#include <QObject>
#include "../support/Shorthands.hpp"
#include "../support/Property.hpp"
#include "ViewportSplitMode.hpp"
#include "Tool.hpp"

namespace Lattice {

namespace Document {
class Document;
class MeshObject;
}

namespace State {

class MeshEditState;

class AppState final : public QObject {
    Q_OBJECT

    LATTICE_AUTO_PROPERTY(bool, isTranslateHandleVisible, setTranslateHandleVisible, true)
    LATTICE_AUTO_PROPERTY(bool, isRotateHandleVisible, setRotateHandleVisible, false)
    LATTICE_AUTO_PROPERTY(bool, isScaleHandleVisible, setScaleHandleVisible, false)

    LATTICE_AUTO_PROPERTY(bool, isVertexSelectable, setVertexSelectable, true)
    LATTICE_AUTO_PROPERTY(bool, isEdgeSelectable, setEdgeSelectable, true)
    LATTICE_AUTO_PROPERTY(bool, isFaceSelectable, setFaceSelectable, true)

    LATTICE_AUTO_PROPERTY(Tool, tool, setTool, Tool::None)

    LATTICE_AUTO_PROPERTY(ViewportSplitMode, viewportSplitMode, setViewportSplit, ViewportSplitMode::Single)

    LATTICE_AUTO_PROPERTY(Opt<SP<MeshEditState>>, meshEditState, setMeshEditState, {})

public:
    AppState();

    auto& document() const { return _document; }

    void startEditing();
    void startEditing(const SP<Document::MeshObject>& object);
    void endEditing();

    void deleteObjects();

    void addPlane();
    void addCube();
    void addCircle();
    void addSphere();
    void addCone();
    void addCylinder();

    void addText();
    void addImagePlane();
    void import();

    void deleteVertices();
    void deleteEdges();
    void deleteFaces();

    void selectAll();
    void deselectAll();

private:
    SP<Document::Document> _document;
};

}
} // namespace Lattice
