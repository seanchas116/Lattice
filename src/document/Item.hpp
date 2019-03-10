#pragma once
#include "../support/Shorthands.hpp"
#include "../support/Location.hpp"
#include <QObject>
#include <nlohmann/json_fwd.hpp>
#include <vector>
#include <functional>
#include <optional>

namespace Lattice {

class Change;

namespace Document {

class Document;

class Item : public QObject, public EnableSharedFromThis<Item> {
    Q_OBJECT
public:
    Opt<SP<Item>> parentItem() const;
    const std::vector<SP<Item>>& childItems() const { return _childItems; }
    Opt<SP<Item>> nextItem() const;
    void appendChildItem(const SP<Item>& item);
    void insertItemBefore(const SP<Item>& item, const Opt<SP<const Item> > &reference);
    void removeChildItem(const SP<Item>& item);
    int index() const;
    std::vector<int> indexPath() const;

    void setName(const std::string& name);
    auto& name() const { return _name; }

    Location location() const { return _location; }
    void setLocation(const Location &location);

    // TODO: provide better way to notify changes
    void notifyChanged() { emit changed(); }
    void notifyChangeFinished() { emit changeFinished(); }

    void forEachDescendant(const Fn<void(const SP<Item> &)> &callback);

    virtual bool canInsertItem(const SP<const Item>& item) const;
    virtual SP<Item> clone() const = 0;
    virtual void toJSON(nlohmann::json& json) const;
    virtual void fromJSON(const nlohmann::json& json);
    virtual Opt<SP<Document>> document() const;

protected:
    void addChange(const SP<Change>& change);

signals:
    void changed();
    void changeFinished();

    void nameChanged(const std::string& name);
    void locationChanged(const Location &location);

    void childItemsAboutToBeInserted(int first, int last);
    void childItemsInserted(int first, int last);
    void childItemsAboutToBeRemoved(int first, int last);
    void childItemsRemoved(int first, int last);

private:
    class NameChange;
    class LocationChange;
    class ChildInsertChange;
    class ChildRemoveChange;

    void setNameInternal(const std::string& name);
    void setLocationInternal(const Location& location);

    void insertItemBeforeInternal(const SP<Item>& item, const Opt<SP<const Item> > &reference);
    void removeChildItemInternal(const SP<Item>& item);

    std::string _name;
    Location _location;
    WP<Item> _parentItem;
    std::vector<SP<Item>> _childItems;
};

}
}
