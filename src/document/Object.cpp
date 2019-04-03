#include "Object.hpp"
#include "Document.hpp"
#include "History.hpp"
#include "../support/JSON.hpp"
#include "../support/OptionalGuard.hpp"
#include "../support/Change.hpp"
#include "../support/PropertyChange.hpp"
#include <QtDebug>
#include <nlohmann/json.hpp>
#include <algorithm>

namespace Lattice {
namespace Document {

class Object::ChildInsertChange : public Change {
public:
    ChildInsertChange(const SP<Object>& parent, const SP<Object>& object, const Opt<SP<const Object>>& reference) :
        _parent(parent),
        _object(object),
        _reference(reference)
    {
    }

    void apply() override {
        _parent->insertObjectBeforeInternal(_object, _reference);
    }

    SP<Change> invert() const override;

private:

    SP<Object> _parent;
    SP<Object> _object;
    Opt<SP<const Object>> _reference;
};

class Object::ChildRemoveChange : public Change {
public:
    ChildRemoveChange(const SP<Object>& parent, const SP<Object>& object) : _parent(parent), _object(object) {
    }

    void apply() override {
        _reference = _object->nextObject();
        _parent->removeChildObjectInternal(_object);
    }

    SP<Change> invert() const override;

private:

    SP<Object> _parent;
    SP<Object> _object;
    Opt<SP<Object>> _reference;
};

SP<Change> Object::ChildInsertChange::invert() const {
    return makeShared<ChildRemoveChange>(_parent, _object);
}

SP<Change> Object::ChildRemoveChange::invert() const {
    return makeShared<ChildInsertChange>(_parent, _object, _reference);
}

Opt<SP<Object> > Object::parentObject() const {
    auto ptr = _parentObject.lock();
    if (ptr) {
        return {ptr};
    } else {
        return {};
    }
}

Opt<SP<Object>> Object::nextObject() const {
    LATTICE_OPTIONAL_GUARD(parent, parentObject(), return {};)

    auto it = std::find(parent->_childObjects.begin(), parent->_childObjects.end(), sharedFromThis());
    if (it == parent->_childObjects.end() || it == parent->_childObjects.end() - 1) {
        return {};
    }
    return *(it + 1);
}

void Object::appendChildObject(const SP<Object> &object) {
    insertObjectBefore(object, {});
}

void Object::insertObjectBefore(const SP<Object> &object, const Opt<SP<const Object>> &reference) {
    addChange(makeShared<Object::ChildInsertChange>(sharedFromThis(), object, reference));
}

void Object::insertObjectBeforeInternal(const SP<Object> &object, const Opt<SP<const Object>> &reference) {
    if (!canInsertObject(object)) {
        throw std::runtime_error("cannot insert object");
    }
    LATTICE_OPTIONAL_LET(oldParent, object->parentObject(),
         oldParent->removeChildObject(object);
    )

    decltype(_childObjects)::iterator it;
    if (reference) {
        it = std::find(_childObjects.begin(), _childObjects.end(), reference);
        if (it == _childObjects.end()) {
            throw std::runtime_error("cannot find reference object");
        }
    } else {
        it = _childObjects.end();
    }
    int index = it - _childObjects.begin();
    emit childObjectsAboutToBeInserted(index, index, {object});
    _childObjects.insert(it, object);
    object->_parentObject = sharedFromThis();
    emit childObjectsInserted(index, index);
}

void Object::removeChildObject(const SP<Object>& object) {
    addChange(makeShared<Object::ChildRemoveChange>(sharedFromThis(), object));
}

void Object::removeChildObjectInternal(const SP<Object>& object) {
    auto it = std::find(_childObjects.begin(), _childObjects.end(), object);
    if (it == _childObjects.end()) {
        throw std::runtime_error("cannot find object");
    }
    int index = it - _childObjects.begin();
    emit childObjectsAboutToBeRemoved(index, index);
    _childObjects.erase(it);
    object->_parentObject.reset();
    emit childObjectsRemoved(index, index, {object});
}

int Object::index() const {
    LATTICE_OPTIONAL_GUARD(parent, _parentObject.lock(), return -1;)

    auto& siblings = parent->_childObjects;
    auto it = std::find(siblings.begin(), siblings.end(), sharedFromThis());
    if (it == siblings.end()) {
        return -1;
    }
    return it - siblings.begin();
}

std::vector<int> Object::indexPath() const {
    LATTICE_OPTIONAL_GUARD(parent, parentObject(), return {};)
    auto path = parent->indexPath();
    path.push_back(index());
    return path;
}

bool Object::canInsertObject(const SP<const Object> &object) const {
    Q_UNUSED(object)
    return true;
}

void Object::toJSON(nlohmann::json &json) const {
    json["name"] = name();
    json["location"] = location();
}

void Object::fromJSON(const nlohmann::json &json) {
    setName(json["name"]);
    setLocation(json["location"]);
}

Opt<SP<Document>> Object::document() const {
    LATTICE_OPTIONAL_GUARD(parent, _parentObject.lock(), return {};)
    return parent->document();
}

void Object::addChange(const SP<Change> &change) {
    LATTICE_OPTIONAL_GUARD(ducument, this->document(), change->apply(); return;);
    ducument->history()->addChange(change);
}

void Object::setName(const std::string &name) {
    if (_name != name) {
        auto change = makeShared<PropertyChange<Object, std::string>>(sharedFromThis(), name, &Object::name, &Object::setNameInternal);
        addChange(change);
    }
}

void Object::setNameInternal(const std::string &name) {
    if (_name != name) {
        _name = name;
        emit nameChanged(name);
    }
}

void Object::setLocation(const Location &location) {
    if (_location != location) {
        auto change = makeShared<PropertyChange<Object, Location>>(sharedFromThis(), location, &Object::location, &Object::setLocationInternal);
        addChange(change);
    }
}

void Object::forEachDescendant(const Fn<void(const SP<Object>&)> &callback) {
    callback(sharedFromThis());
    for (auto& child : _childObjects) {
        child->forEachDescendant(callback);
    }
}

void Object::setLocationInternal(const Location &location) {
    if (_location != location) {
        _location = location;
        emit locationChanged(location);
    }
}

}
} // namespace Lattice
