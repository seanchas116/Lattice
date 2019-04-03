#pragma once

#include <QObject>
#include "../support/Shorthands.hpp"
#include "../support/Property.hpp"
#include "ViewportSplitMode.hpp"
#include "Preferences.hpp"
#include "Tool.hpp"

namespace Lattice {

namespace Document {
class Document;
}

namespace State {

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
public:
    AppState();

    const auto& document() const { return _document; }
    const auto& preferences() const { return _preferences; }

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
    SP<Preferences> _preferences;
};

}
} // namespace Lattice
