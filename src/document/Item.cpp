#include "Item.hpp"
#include "Document.hpp"
#include "History.hpp"
#include "../support/JSON.hpp"
#include <QtDebug>
#include <nlohmann/json.hpp>
#include <algorithm>

namespace Lattice {

class Item::ChildInsertChange : public Change {
public:
    ChildInsertChange(const SP<Item>& parent, const SP<Item>& item, const SP<const Item>& reference) :
        _parent(parent),
        _item(item),
        _reference(reference)
    {
    }

    void redo() override {
        _parent->insertItemBeforeInternal(_item, _reference);
    }

    void undo() override {
        _parent->removeChildItemInternal(_item);
    }

private:

    SP<Item> _parent;
    SP<Item> _item;
    SP<const Item> _reference;
};

class Item::ChildRemoveChange : public Change {
public:
    ChildRemoveChange(const SP<Item>& parent, const SP<Item>& item) : _parent(parent), _item(item) {
        _reference = _item->nextItem();
    }

    void redo() override {
        _parent->removeChildItemInternal(_item);
    }

    void undo() override {
        _parent->insertItemBeforeInternal(_item, _reference);
    }

private:

    SP<Item> _parent;
    SP<Item> _item;
    SP<Item> _reference;
};

SP<Item> Item::nextItem() const {
    auto parent = parentItem();
    if (!parent) {
        return {};
    }
    auto it = std::find(parent->_childItems.begin(), parent->_childItems.end(), shared_from_this());
    if (it == parent->_childItems.end() || it == parent->_childItems.end() - 1) {
        return {};
    }
    return *(it + 1);
}

void Item::appendChildItem(const SP<Item> &item) {
    insertItemBefore(item, nullptr);
}

void Item::insertItemBefore(const SP<Item> &item, const SP<const Item> &reference) {
    addChange(std::make_shared<Item::ChildInsertChange>(shared_from_this(), item, reference));
}

void Item::insertItemBeforeInternal(const SP<Item> &item, const SP<const Item> &reference) {
    if (!canInsertItem(item)) {
        throw std::runtime_error("cannot insert item");
    }
    auto oldParent = item->parentItem();
    if (oldParent) {
        oldParent->removeChildItem(item);
    }

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
    item->_parentItem = shared_from_this();
    emit childItemsInserted(index, index);
}

void Item::removeChildItem(const SP<Item>& item) {
    addChange(std::make_shared<Item::ChildRemoveChange>(shared_from_this(), item));
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
    auto parent = _parentItem.lock();
    if (parent) {
        auto& siblings = parent->_childItems;
        auto it = std::find(siblings.begin(), siblings.end(), shared_from_this());
        if (it == siblings.end()) {
            return -1;
        }
        return it - siblings.begin();
    }
    return -1;
}

std::vector<int> Item::indexPath() const {
    auto parent = parentItem();
    if (!parent) {
        return {};
    }
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

SP<Document> Item::document() const {
    auto parent = _parentItem.lock();
    if (!parent) {
        return {};
    }
    return parent->document();
}

void Item::addChange(const SP<Change> &change) {
    auto document = this->document();
    if (document) {
        document->history()->addChange(change);
    } else {
        change->redo(); // only apply change
    }
}

class Item::NameChange : public Change {
public:
    NameChange(SP<Item> item, QString name) : _item(std::move(item)), _name(std::move(name)) {
        _oldName = _item->name();
    }

    void redo() override {
        _item->setNameInternal(_name);
    }

    void undo() override {
        _item->setNameInternal(_oldName);
    }

    bool mergeWith(const SP<const Change>& other) override {
        auto change = std::dynamic_pointer_cast<const NameChange>(other);
        if (change && change->_item == _item) {
            _name = change->_name;
            return true;
        }
        return false;
    }

private:
    SP<Item> _item;
    QString _name;
    QString _oldName;
};

class Item::LocationChange : public Change {
public:
    LocationChange(SP<Item> item, Location location) : _item(std::move(item)), _location(std::move(location)) {
        _oldLocation = item->location();
    }

    void redo() override {
        _item->setLocationInternal(_location);
    }

    void undo() override {
        _item->setLocationInternal(_oldLocation);
    }

    bool mergeWith(const SP<const Change>& other) override {
        auto change = std::dynamic_pointer_cast<const LocationChange>(other);
        if (change && change->_item == _item) {
            _location = change->_location;
            return true;
        }
        return false;
    }

private:
    SP<Item> _item;
    Location _location;
    Location _oldLocation;
};

void Item::setName(const QString &name) {
    addChange(std::make_shared<NameChange>(shared_from_this(), name));
}

void Item::setNameInternal(const QString &name) {
    if (_name != name) {
        _name = name;
        emit nameChanged(name);
    }
}

void Item::setLocation(const Location &location) {
    addChange(std::make_shared<LocationChange>(shared_from_this(), location));
}

void Item::setLocationInternal(const Location &location) {
    if (_location != location) {
        _location = location;
        emit locationChanged(location);
    }
}

} // namespace Shapecraft
