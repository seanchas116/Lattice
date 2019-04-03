#include "Document.hpp"
#include "MeshObject.hpp"
#include "History.hpp"
#include "../support/Debug.hpp"
#include "../support/OptionalGuard.hpp"

namespace Lattice {
namespace Document {

namespace {

class RootItem final : public Object {
public:
    RootItem(Document* document) : _document(document) {}

    SP<Object> clone() const override {
        throw std::runtime_error("RootItem cannot be copied");
    }

    bool canInsertItem(const SP<const Object>& item) const override {
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
    _rootObject(makeShared<RootItem>(this)),
    _history(makeShared<History>())
{
    watchChildrenInsertRemove(_rootObject);
    connect(this, &Document::editedObjectChanged, this, [this] {
        emit isEditingChanged(isEditing());
    });
}

void Document::setCurrentObject(const Opt<SP<Object> > &item) {
    if (item != _currentObject) {
        _currentObject = item;
        emit currentObjectChanged(item);
    }
}

void Document::setEditedObject(const Opt<SP<MeshObject> > &item) {
    if (item != _editedObject) {
        _editedObject = item;
        emit editedObjectChanged(item);
    }
}

void Document::setIsEditing(bool isEditing) {
    if (isEditing) {
        auto item = dynamicPointerCast<MeshObject>(_currentObject);
        setEditedObject(item);
    } else {
        setEditedObject(std::nullopt);
    }
}

void Document::setSelectedObjects(const std::unordered_set<SP<Object>> &items) {
    if (_selectedObjects != items) {
        _selectedObjects = items;
        emit selectedObjectsChanged(items);
    }
}

void Document::selectObject(const SP<Object> &item, bool append) {
    std::unordered_set<SP<Object>> items;
    if (append) {
        items = _selectedObjects;
    }
    items.insert(item);
    setSelectedObjects(items);
    setCurrentObject(item);
}

void Document::insertObjectToCurrentPosition(const SP<Object> &item) {
    // TODO: better insertion positon
    _rootObject->appendChildItem(item);
}

void Document::deleteSelectedObjects() {
    auto items = _selectedObjects;
    for (auto& item : items) {
        LATTICE_OPTIONAL_GUARD(parent, item->parentItem(), continue;)
        parent->removeChildItem(item);
    }
}

void Document::watchChildrenInsertRemove(const SP<Object> &item) {
    auto itemPtr = item.get();
    connect(itemPtr, &Object::childItemsAboutToBeInserted, this, [this] (int, int, const auto& items) {
        for (auto& item : items) {
            emit objectAboutToBeInserted(item);
        }
    });
    connect(itemPtr, &Object::childItemsInserted, this, [this, itemPtr] (int first, int last) {
        for (int i = first; i <= last; ++i) {
            auto& child = itemPtr->childItems()[i];
            watchChildrenInsertRemove(child);
            emit objectInserted(child);
        }
    });
    connect(itemPtr, &Object::childItemsAboutToBeRemoved, this, [this, itemPtr] (int first, int last) {
        for (int i = first; i <= last; ++i) {
            auto& child = itemPtr->childItems()[i];
            emit objectAboutToBeRemoved(child);
        }
    });
    connect(itemPtr, &Object::childItemsRemoved, this, [this] (int, int, const auto& items) {
        for (auto& item : items) {
            emit objectRemoved(item);
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
