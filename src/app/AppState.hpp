#pragma once

#include <QObject>
#include "../support/Pointer.hpp"

namespace Lattice {

class Document;

class AppState final : public QObject {
    Q_OBJECT
public:
    AppState();

    const auto& document() const { return _document; }

    bool isVertexVisible() const { return _isVertexVisible; }
    void setIsVertexVisible(bool isVertexVisible);

    bool isEdgeVisible() const { return _isEdgeVisible; }
    void setIsEdgeVisible(bool isEdgeVisible);

    bool isFaceVisible() const { return _isFaceVisible; }
    void setIsFaceVisible(bool isFaceVisible);

signals:
    void isVertexVisibleChanged(bool isVertexVisible);
    void isEdgeVisibleChanged(bool isEdgeVisible);
    void isFaceVisibleChanged(bool isFaceVisible);

private:
    SP<Document> _document;

    bool _isVertexVisible = true;
    bool _isEdgeVisible = true;
    bool _isFaceVisible = true;
};

} // namespace Lattice
