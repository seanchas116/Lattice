#include "AppState.hpp"
#include "../document/Document.hpp"
#include "../document/MeshItem.hpp"

namespace Lattice {

AppState::AppState() {
    _document = std::make_shared<Document>();
    _document->addInitialItems();
    _document->setCurrentItem(_document->rootItem()->childItems()[0]);
}

void AppState::setIsVertexVisible(bool isVertexVisible) {
    if (_isVertexVisible != isVertexVisible) {
        _isVertexVisible = isVertexVisible;
        emit isVertexVisibleChanged(isVertexVisible);
    }
}

void AppState::setIsEdgeVisible(bool isEdgeVisible) {
    if (_isEdgeVisible != isEdgeVisible) {
        _isEdgeVisible = isEdgeVisible;
        emit isEdgeVisibleChanged(isEdgeVisible);
    }
}

void AppState::setIsFaceVisible(bool isFaceVisible) {
    if (_isFaceVisible != isFaceVisible) {
        _isFaceVisible = isFaceVisible;
        emit isFaceVisibleChanged(isFaceVisible);
    }
}

} // namespace Lattice
