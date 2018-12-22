#include "AppState.hpp"
#include "../document/Document.hpp"
#include "../document/MeshItem.hpp"

namespace Lattice {

AppState::AppState() {
    _document = std::make_shared<Document>();
    _document->addInitialItems();
    setCurrentItem(_document->rootItem()->childItems()[0]);
}

SP<MeshItem> AppState::currentMeshItem() const {
    return std::dynamic_pointer_cast<MeshItem>(_currentItem);
}

void AppState::setCurrentItem(const SP<Item> &item) {
    if (item != _currentItem) {
        _currentItem = item;
        emit currentItemChanged(item);
        emit currentMeshItemChanged(currentMeshItem());
    }
}

} // namespace Lattice
