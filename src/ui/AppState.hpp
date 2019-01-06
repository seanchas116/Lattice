#pragma once

#include <QObject>
#include "../support/Pointer.hpp"
#include "Property.hpp"

namespace Lattice::Document {
class Document;
}

namespace Lattice::UI {

class AppState final : public QObject {
    Q_OBJECT
    LATTICE_AUTO_PROPERTY(bool, isVertexVisible, setIsVertexVisible, true)
    LATTICE_AUTO_PROPERTY(bool, isEdgeVisible, setIsEdgeVisible, true)
    LATTICE_AUTO_PROPERTY(bool, isFaceVisible, setIsFaceVisible, true)
public:
    AppState();

    const auto& document() const { return _document; }

    void deleteItems();

    void addPlane();
    void addCube();
    void addCircle();
    void addSphere();
    void addCone();
    void addCylinder();

    void addText();
    void addImagePlane();
    void import();

private:
    SP<Document::Document> _document;
};

} // namespace Lattice
