#include "AppState.hpp"
#include "../document/Document.hpp"
#include "../document/MeshItem.hpp"
#include "../document/Mesh.hpp"

using namespace glm;

namespace Lattice {

AppState::AppState() {
    _document = std::make_shared<Document>();
    _document->addInitialItems();
    auto initialItem = _document->rootItem()->childItems()[0];
    _document->setCurrentItem(initialItem);

    auto mesh = std::dynamic_pointer_cast<MeshItem>(initialItem)->mesh();
    {
        auto v1 = mesh->addVertex(vec3(-1, 0, -1), vec2(0, 0));
        auto v2 = mesh->addVertex(vec3(-1, 0, 1), vec2(0, 1));
        auto v3 = mesh->addVertex(vec3(1, 0, 1), vec2(1, 1));
        auto v4 = mesh->addVertex(vec3(1, 0, -1), vec2(1, 0));
        auto m = mesh->addMaterial();
        mesh->addFace({v1, v2, v3, v4}, m);
    }
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
