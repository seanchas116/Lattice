#include "Item.hpp"
#include "Document.hpp"
#include "History.hpp"
#include "../support/JSON.hpp"
#include "../support/OptionalGuard.hpp"
#include "../support/Change.hpp"
#include <QtDebug>
#include <nlohmann/json.hpp>
#include <algorithm>

namespace Lattice {
namespace Document {

class Item::ChildInsertChange : public Change {
public:
    ChildInsertChange(const SP<Item>& parent, const SP<Item>& item, const Opt<SP<const Item>>& reference) :
        _parent(parent),
        _item(item),
        _reference(reference)
    {
    }

    void apply() override {
        _parent->insertItemBeforeInternal(_item, _reference);
    }

    SP<Change> invert() const override;

private:

    SP<Item> _parent;
    SP<Item> _item;
    Opt<SP<const Item>> _reference;
};

class Item::ChildRemoveChange : public Change {
public:
    ChildRemoveChange(const SP<Item>& parent, const SP<Item>& item) : _parent(parent), _item(item) {
    }

    void apply() override {
        _reference = _item->nextItem();
        _parent->removeChildItemInternal(_item);
    }

    SP<Change> invert() const override;

private:

    SP<Item> _parent;
    SP<Item> _item;
    Opt<SP<Item>> _reference;
};

SP<Change> Item::ChildInsertChange::invert() const {
    return makeShared<ChildRemoveChange>(_parent, _item);
}

SP<Change> Item::ChildRemoveChange::invert() const {
    return makeShared<ChildInsertChange>(_parent, _item, _reference);
}

Opt<SP<Item> > Item::parentItem() const {
    auto ptr = _parentItem.lock();
    if (ptr) {
        return {ptr};
    } else {
        return {};
    }
}

Opt<SP<Item>> Item::nextItem() const {
    LATTICE_OPTIONAL_GUARD(parent, parentItem(), return {};)

    auto it = std::find(parent->_childItems.begin(), parent->_childItems.end(), sharedFromThis());
    if (it == parent->_childItems.end() || it == parent->_childItems.end() - 1) {
        return {};
    }
    return *(it + 1);
}

void Item::appendChildItem(const SP<Item> &item) {
    insertItemBefore(item, {});
}

void Item::insertItemBefore(const SP<Item> &item, const Opt<SP<const Item>> &reference) {
    addChange(makeShared<Item::ChildInsertChange>(sharedFromThis(), item, reference));
}

void Item::insertItemBeforeInternal(const SP<Item> &item, const Opt<SP<const Item>> &reference) {
    if (!canInsertItem(item)) {
        throw std::runtime_error("cannot insert item");
    }
    LATTICE_OPTIONAL_LET(oldParent, item->parentItem(),
         oldParent->removeChildItem(item);
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
    emit childItemsAboutToBeInserted(index, index);
    _childItems.insert(it, item);
    item->_parentItem = sharedFromThis();
    emit childItemsInserted(index, index);
}

void Item::removeChildItem(const SP<Item>& item) {
    addChange(makeShared<Item::ChildRemoveChange>(sharedFromThis(), item));
}

void Item::removeChildItemInternal(const SP<Item>& item) {
    auto it = std::find(_childItems.begin(), _childItems.end(), item);
    if (it == _childItems.end()) {
        throw std::runtime_error("cannot find item");
    }
    int index = it - _childItems.begin();
    emit childItemsAboutToBeRemoved(index, index);
    _childItems.erase(it);
    item->_parentItem.reset();
    emit childItemsRemoved(index, index);
}

int Item::index() const {
    LATTICE_OPTIONAL_GUARD(parent, _parentItem.lock(), return -1;)

    auto& siblings = parent->_childItems;
    auto it = std::find(siblings.begin(), siblings.end(), sharedFromThis());
    if (it == siblings.end()) {
        return -1;
    }
    return it - siblings.begin();
}

std::vector<int> Item::indexPath() const {
    LATTICE_OPTIONAL_GUARD(parent, parentItem(), return {};)
    auto path = parent->indexPath();
    path.push_back(index());
    return path;
}

bool Item::canInsertItem(const SP<const Item> &item) const {
    Q_UNUSED(item)
    return true;
}

void Item::toJSON(nlohmann::json &json) const {
    json["name"] = name();
    json["location"] = location();
}

void Item::fromJSON(const nlohmann::json &json) {
    setName(json["name"]);
    setLocation(json["location"]);
}

Opt<SP<Document>> Item::document() const {
    LATTICE_OPTIONAL_GUARD(parent, _parentItem.lock(), return {};)
    return parent->document();
}

void Item::addChange(const SP<Change> &change) {
    LATTICE_OPTIONAL_GUARD(ducument, this->document(), change->apply(); return;);
    ducument->history()->addChange(change);
}

class Item::NameChange : public Change {
public:
    NameChange(const SP<Item>& item, const std::string& name) : _item(item), _name(name) {
    }

    void apply() override {
        _oldName = _item->name();
        _item->setNameInternal(_name);
    }

    SP<Change> invert() const override {
        return makeShared<NameChange>(_item, _oldName);
    }

    bool mergeWith(const SP<const Change>& other) override {
        LATTICE_OPTIONAL_GUARD(change, dynamicPointerCast<const NameChange>(other), return false;)
        if (change->_item != _item) { return false; }
        _name = change->_name;
        return true;
    }

private:
    SP<Item> _item;
    std::string _name;
    std::string _oldName;
};

class Item::LocationChange : public Change {
public:
    LocationChange(const SP<Item>& item, const Location& location) : _item(item), _location(location) {
    }

    void apply() override {
        _oldLocation = _item->location();
        _item->setLocationInternal(_location);
    }

    SP<Change> invert() const override {
        return makeShared<LocationChange>(_item, _oldLocation);
    }

    bool mergeWith(const SP<const Change>& other) override {
        LATTICE_OPTIONAL_GUARD(change, dynamicPointerCast<const LocationChange>(other), return false;)
        if (change->_item != _item) { return false; }
        _location = change->_location;
        return true;
    }

private:
    SP<Item> _item;
    Location _location;
    Location _oldLocation;
};

void Item::setName(const std::string &name) {
    if (_name != name) {
        addChange(makeShared<NameChange>(sharedFromThis(), name));
    }
}

void Item::setNameInternal(const std::string &name) {
    if (_name != name) {
        _name = name;
        emit nameChanged(name);
    }
}

void Item::setLocation(const Location &location) {
    if (_location != location) {
        addChange(makeShared<LocationChange>(sharedFromThis(), location));
    }
}

void Item::forEachDescendant(const Fn<void(const SP<Item>&)> &callback) {
    callback(sharedFromThis());
    for (auto& child : _childItems) {
        child->forEachDescendant(callback);
    }
}

void Item::setLocationInternal(const Location &location) {
    if (_location != location) {
        _location = location;
        emit locationChanged(location);
    }
}

}
} // namespace Lattice
