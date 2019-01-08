#include "Document.hpp"
#include "MeshItem.hpp"
#include "History.hpp"
#include "../support/Debug.hpp"
#include "../support/OptionalGuard.hpp"

namespace Lattice::Document {

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
    meshItem->setName(tr("Mesh").toStdString());

    _rootItem->appendChildItem(meshItem);
    _selectedItems = {meshItem};
}

std::optional<SP<MeshItem> > Document::currentMeshItem() const {
    if (!_currentItem) {
        return {};
    }
    auto meshItem = std::dynamic_pointer_cast<MeshItem>(*_currentItem);
    if (!meshItem) {
        return {};
    }
    return meshItem;
}

void Document::setCurrentItem(const std::optional<SP<Item> > &item) {
    if (item != _currentItem) {
        _currentItem = item;
        emit currentItemChanged(item);
        emit currentMeshItemChanged(currentMeshItem());
    }
}

void Document::setSelectedItems(const std::unordered_set<SP<Item>> &items) {
    if (_selectedItems != items) {
        _selectedItems = items;
        emit selectedItemsChanged(items);
    }
}

void Document::selectItem(const SP<Item> &item, bool append) {
    std::unordered_set<SP<Item>> items;
    if (append) {
        items = _selectedItems;
    }
    items.insert(item);
    setSelectedItems(items);
    setCurrentItem(item);
}

void Document::insertItemToCurrentPosition(const SP<Item> &item) {
    // TODO: better insertion positon
    _rootItem->appendChildItem(item);
}

void Document::deleteSelectedItems() {
    auto items = _selectedItems;
    for (auto& item : items) {
        LATTICE_OPTIONAL_GUARD(parent, item->parentItem(), continue;)
        parent->removeChildItem(item);
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
