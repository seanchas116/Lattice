#include "Document.hpp"
#include "../support/Debug.hpp"
#include "../support/OptionalGuard.hpp"
#include "History.hpp"
#include "ImageManager.hpp"
#include "MeshObject.hpp"

namespace Lattice {
namespace Document {

namespace {

class RootObject final : public Object {
  public:
    RootObject(Document *document) : _document(document) {}

    SP<Object> clone() const override {
        throw std::runtime_error("RootObject cannot be copied");
    }

    bool canInsertObject(const SP<const Object> &) const override {
        return true;
    }

    Opt<SP<Document>> document() const override {
        return _document->sharedFromThis();
    }

  private:
    Document *_document;
};

} // namespace

Document::Document() : _rootObject(makeShared<RootObject>(this)),
                       _imageManager(makeShared<ImageManager>()),
                       _history(makeShared<History>()) {
    watchChildrenInsertRemove(_rootObject);
}

void Document::setCurrentObject(const Opt<SP<Object>> &object) {
    if (object != _currentObject) {
        _currentObject = object;
        emit currentObjectChanged(object);
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
    auto objects = _selectedObjects;
    for (auto &object : objects) {
        LATTICE_OPTIONAL_GUARD(parent, object->parentObject(), continue;)
        parent->removeChildObject(object);
    }
}

void Document::watchChildrenInsertRemove(const SP<Object> &object) {
    auto objectPtr = object.get();
    connect(objectPtr, &Object::childObjectsAboutToBeInserted, this, [this](int, int, const auto &objects) {
        for (auto &object : objects) {
            emit objectAboutToBeInserted(object);
        }
    });
    connect(objectPtr, &Object::childObjectsInserted, this, [this, objectPtr](int first, int last) {
        for (int i = first; i <= last; ++i) {
            auto &child = objectPtr->childObjects()[i];
            watchChildrenInsertRemove(child);
            emit objectInserted(child);
        }
    });
    connect(objectPtr, &Object::childObjectsAboutToBeRemoved, this, [this, objectPtr](int first, int last) {
        for (int i = first; i <= last; ++i) {
            auto &child = objectPtr->childObjects()[i];
            emit objectAboutToBeRemoved(child);
        }
    });
    connect(objectPtr, &Object::childObjectsRemoved, this, [this](int, int, const auto &objects) {
        for (auto &object : objects) {
            emit objectRemoved(object);
        }
    });
}

} // namespace Document
} // namespace Lattice
