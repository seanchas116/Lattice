#include "Document.hpp"
#include "MeshObject.hpp"
#include "History.hpp"
#include "../support/Debug.hpp"
#include "../support/OptionalGuard.hpp"

namespace Lattice {
namespace Document {

namespace {

class RootObject final : public Object {
public:
    RootObject(Document* document) : _document(document) {}

    SP<Object> clone() const override {
        throw std::runtime_error("RootObject cannot be copied");
    }

    bool canInsertItem(const SP<const Object>&) const override {
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
    _rootObject(makeShared<RootObject>(this)),
    _history(makeShared<History>())
{
    watchChildrenInsertRemove(_rootObject);
    connect(this, &Document::editedObjectChanged, this, [this] {
        emit isEditingChanged(isEditing());
    });
}

void Document::setCurrentObject(const Opt<SP<Object> > &object) {
    if (object != _currentObject) {
        _currentObject = object;
        emit currentObjectChanged(object);
    }
}

void Document::setEditedObject(const Opt<SP<MeshObject> > &object) {
    if (object != _editedObject) {
        _editedObject = object;
        emit editedObjectChanged(object);
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

void Document::setSelectedObjects(const std::unordered_set<SP<Object>> &objects) {
    if (_selectedObjects != objects) {
        _selectedObjects = objects;
        emit selectedObjectsChanged(objects);
    }
}

void Document::selectObject(const SP<Object> &object, bool append) {
    std::unordered_set<SP<Object>> items;
    if (append) {
        items = _selectedObjects;
    }
    items.insert(object);
    setSelectedObjects(items);
    setCurrentObject(object);
}

void Document::insertObjectToCurrentPosition(const SP<Object> &object) {
    // TODO: better insertion positon
    _rootObject->appendChildItem(object);
}

void Document::deleteSelectedObjects() {
    auto items = _selectedObjects;
    for (auto& item : items) {
        LATTICE_OPTIONAL_GUARD(parent, item->parentItem(), continue;)
        parent->removeChildItem(item);
    }
}

void Document::watchChildrenInsertRemove(const SP<Object> &object) {
    auto itemPtr = object.get();
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
