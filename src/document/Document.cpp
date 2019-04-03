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

    bool canInsertObject(const SP<const Object>&) const override {
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
        auto object = dynamicPointerCast<MeshObject>(_currentObject);
        setEditedObject(object);
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
    std::unordered_set<SP<Object>> objects;
    if (append) {
        objects = _selectedObjects;
    }
    objects.insert(object);
    setSelectedObjects(objects);
    setCurrentObject(object);
}

void Document::insertObjectToCurrentPosition(const SP<Object> &object) {
    // TODO: better insertion positon
    _rootObject->appendChildObject(object);
}

void Document::deleteSelectedObjects() {
    auto& objects = _selectedObjects;
    for (auto& object : objects) {
        LATTICE_OPTIONAL_GUARD(parent, object->parentObject(), continue;)
        parent->removeChildObject(object);
    }
}

void Document::watchChildrenInsertRemove(const SP<Object> &object) {
    auto objectPtr = object.get();
    connect(objectPtr, &Object::childObjectsAboutToBeInserted, this, [this] (int, int, const auto& objects) {
        for (auto& object : objects) {
            emit objectAboutToBeInserted(object);
        }
    });
    connect(objectPtr, &Object::childObjectsInserted, this, [this, objectPtr] (int first, int last) {
        for (int i = first; i <= last; ++i) {
            auto& child = objectPtr->childObjects()[i];
            watchChildrenInsertRemove(child);
            emit objectInserted(child);
        }
    });
    connect(objectPtr, &Object::childObjectsAboutToBeRemoved, this, [this, objectPtr] (int first, int last) {
        for (int i = first; i <= last; ++i) {
            auto& child = objectPtr->childObjects()[i];
            emit objectAboutToBeRemoved(child);
        }
    });
    connect(objectPtr, &Object::childObjectsRemoved, this, [this] (int, int, const auto& objects) {
        for (auto& object : objects) {
            emit objectRemoved(object);
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
