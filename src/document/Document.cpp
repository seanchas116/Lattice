#include "Document.hpp"
#include "MeshItem.hpp"
#include "History.hpp"

namespace Lattice {

namespace {

class RootItem final : public Item {
public:
    RootItem(Document* document) : _document(document) {}

    SP<Item> clone() const override {
        throw std::runtime_error("RootItem cannot be copied");
    }

    bool canInsertItem(const SP<const Item>& item) const override {
        Q_UNUSED(item)
        return true;
    }

    SP<Document> document() const override {
        return _document->shared_from_this();
    }

private:
    Document* _document;
};

}

Document::Document() {
    _history = std::make_shared<History>();

    _rootItem = std::make_shared<RootItem>(this);
    watchChildrenInsertRemove(_rootItem);
}

void Document::addInitialItems() {
    // Add example item
    auto meshItem = std::make_shared<MeshItem>();
    meshItem->setName(tr("Mesh"));

    _rootItem->appendChildItem(meshItem);
    _selectedItems = {meshItem};
}

SP<MeshItem> Document::currentMeshItem() const {
    return std::dynamic_pointer_cast<MeshItem>(_currentItem);
}

void Document::setCurrentItem(const SP<Item> &item) {
    if (item != _currentItem) {
        _currentItem = item;
        emit currentItemChanged(item);
        emit currentMeshItemChanged(currentMeshItem());
    }
}

void Document::setSelectedItems(const std::vector<SP<Item>> &items) {
    if (_selectedItems != items) {
        _selectedItems = items;
        emit selectedItemsChanged(items);
    }
}

void Document::insertItemToCurrentPosition(const SP<Item> &item) {
    // TODO: better insertion positon
    _rootItem->appendChildItem(item);
}

void Document::deleteSelectedItems() {
    for (auto& item : _selectedItems) {
        auto parent = item->parentItem();
        if (parent) {
            parent->removeChildItem(item);
        }
    }
}

void Document::watchChildrenInsertRemove(const SP<Item> &item) {
    auto itemPtr = item.get();
    connect(itemPtr, &Item::childItemsInserted, this, [this, itemPtr] (int first, int last) {
        for (int i = first; i <= last; ++i) {
            auto& child = itemPtr->childItems()[i];
            watchChildrenInsertRemove(child);
            emit itemInserted(child);
        }
    });
    connect(itemPtr, &Item::childItemsAboutToBeRemoved, this, [this, itemPtr] (int first, int last) {
        for (int i = first; i <= last; ++i) {
            auto& child = itemPtr->childItems()[i];
            emit itemAboutToBeRemoved(child);
        }
    });
}

} // namespace Lattice
