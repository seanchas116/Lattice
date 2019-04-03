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
    ChildInsertChange(const SP<Object>& parent, const SP<Object>& item, const Opt<SP<const Object>>& reference) :
        _parent(parent),
        _item(item),
        _reference(reference)
    {
    }

    void apply() override {
        _parent->insertObjectBeforeInternal(_item, _reference);
    }

    SP<Change> invert() const override;

private:

    SP<Object> _parent;
    SP<Object> _item;
    Opt<SP<const Object>> _reference;
};

class Object::ChildRemoveChange : public Change {
public:
    ChildRemoveChange(const SP<Object>& parent, const SP<Object>& item) : _parent(parent), _item(item) {
    }

    void apply() override {
        _reference = _item->nextObject();
        _parent->removeChildItemInternal(_item);
    }

    SP<Change> invert() const override;

private:

    SP<Object> _parent;
    SP<Object> _item;
    Opt<SP<Object>> _reference;
};

SP<Change> Object::ChildInsertChange::invert() const {
    return makeShared<ChildRemoveChange>(_parent, _item);
}

SP<Change> Object::ChildRemoveChange::invert() const {
    return makeShared<ChildInsertChange>(_parent, _item, _reference);
}

Opt<SP<Object> > Object::parentObject() const {
    auto ptr = _parentItem.lock();
    if (ptr) {
        return {ptr};
    } else {
        return {};
    }
}

Opt<SP<Object>> Object::nextObject() const {
    LATTICE_OPTIONAL_GUARD(parent, parentObject(), return {};)

    auto it = std::find(parent->_childItems.begin(), parent->_childItems.end(), sharedFromThis());
    if (it == parent->_childItems.end() || it == parent->_childItems.end() - 1) {
        return {};
    }
    return *(it + 1);
}

void Object::appendChildObject(const SP<Object> &item) {
    insertObjectBefore(item, {});
}

void Object::insertObjectBefore(const SP<Object> &item, const Opt<SP<const Object>> &reference) {
    addChange(makeShared<Object::ChildInsertChange>(sharedFromThis(), item, reference));
}

void Object::insertObjectBeforeInternal(const SP<Object> &item, const Opt<SP<const Object>> &reference) {
    if (!canInsertObject(item)) {
        throw std::runtime_error("cannot insert item");
    }
    LATTICE_OPTIONAL_LET(oldParent, item->parentObject(),
         oldParent->removeChildObject(item);
    )

    decltype(_childItems)::iterator it;
    if (reference) {
        it = std::find(_childItems.begin(), _childItems.end(), reference);
        if (it == _childItems.end()) {
            throw std::runtime_error("cannot find reference item");
        }
    } else {
        it = _childItems.end();
    }
    int index = it - _childItems.begin();
    emit childObjectsAboutToBeInserted(index, index, {item});
    _childItems.insert(it, item);
    item->_parentItem = sharedFromThis();
    emit childObjectsInserted(index, index);
}

void Object::removeChildObject(const SP<Object>& item) {
    addChange(makeShared<Object::ChildRemoveChange>(sharedFromThis(), item));
}

void Object::removeChildItemInternal(const SP<Object>& item) {
    auto it = std::find(_childItems.begin(), _childItems.end(), item);
    if (it == _childItems.end()) {
        throw std::runtime_error("cannot find item");
    }
    int index = it - _childItems.begin();
    emit childObjectsAboutToBeRemoved(index, index);
    _childItems.erase(it);
    item->_parentItem.reset();
    emit childObjectsRemoved(index, index, {item});
}

int Object::index() const {
    LATTICE_OPTIONAL_GUARD(parent, _parentItem.lock(), return -1;)

    auto& siblings = parent->_childItems;
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

bool Object::canInsertObject(const SP<const Object> &item) const {
    Q_UNUSED(item)
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
    LATTICE_OPTIONAL_GUARD(parent, _parentItem.lock(), return {};)
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
    for (auto& child : _childItems) {
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
