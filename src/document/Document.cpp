#include "Document.hpp"
#include "MeshItem.hpp"
#include "History.hpp"
#include "../support/Debug.hpp"
#include "../support/OptionalGuard.hpp"

namespace Lattice {
namespace Document {

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

    Opt<SP<Document>> document() const override {
        return _document->sharedFromThis();
    }

private:
    Document* _document;
};

}

Document::Document() :
    _rootItem(makeShared<RootItem>(this)),
    _history(makeShared<History>())
{
    watchChildrenInsertRemove(_rootItem);
    connect(this, &Document::editedItemChanged, this, [this] {
        emit isEditingChanged(isEditing());
    });
}

void Document::setCurrentItem(const Opt<SP<Item> > &item) {
    if (item != _currentItem) {
        _currentItem = item;
        emit currentItemChanged(item);
    }
}

void Document::setEditedItem(const Opt<SP<MeshItem> > &item) {
    if (item != _editedItem) {
        _editedItem = item;
        emit editedItemChanged(item);
    }
}

void Document::setIsEditing(bool isEditing) {
    if (isEditing) {
        auto item = dynamicPointerCast<MeshItem>(_currentItem);
        setEditedItem(item);
    } else {
        setEditedItem(std::nullopt);
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
    connect(itemPtr, &Item::childItemsAboutToBeInserted, this, [this] (int, int, const auto& items) {
        for (auto& item : items) {
            emit itemAboutToBeInserted(item);
        }
    });
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
    connect(itemPtr, &Item::childItemsRemoved, this, [this] (int, int, const auto& items) {
        for (auto& item : items) {
            emit itemRemoved(item);
        }
    });
}

void Document::setMeshSelection(const Mesh::MeshFragment &meshSelection) {
    if (_meshSelection == meshSelection) {
        return;
    }
    _meshSelection = meshSelection;
    emit meshSelectionChanged(meshSelection);
}

}
} // namespace Lattice
