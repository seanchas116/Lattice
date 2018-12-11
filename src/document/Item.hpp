#pragma once
#include "../support/Pointer.hpp"
#include "../support/Location.hpp"
#include <QObject>
#include <vector>
#include <nlohmann/json_fwd.hpp>

namespace Lattice {

class Document;
class Change;

class Item : public QObject, public std::enable_shared_from_this<Item> {
    Q_OBJECT
public:
    SP<Item> parentItem() const { return _parentItem.lock(); }
    const std::vector<SP<Item>>& childItems() const { return _childItems; }
    SP<Item> nextItem() const;
    void appendChildItem(const SP<Item>& item);
    void insertItemBefore(const SP<Item>& item, const SP<const Item>& reference);
    void removeChildItem(const SP<Item>& item);
    int index() const;
    std::vector<int> indexPath() const;

    void setName(const QString& name);
    QString name() const { return _name; }

    Location location() const { return _location; }
    void setLocation(const Location &location);

    // TODO: provide better way to notify changes
    void notifyChanged() { emit changed(); }
    void notifyChangeFinished() { emit changeFinished(); }

    virtual bool canInsertItem(const SP<const Item>& item) const;
    virtual SP<Item> clone() const = 0;
    virtual void toJSON(nlohmann::json& json) const;
    virtual void fromJSON(const nlohmann::json& json);
    virtual SP<Document> document() const;

protected:
    void addChange(const SP<Change>& change);

signals:
    void changed();
    void changeFinished();

    void nameChanged(const QString& name);
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

    void setNameInternal(const QString& name);
    void setLocationInternal(const Location& location);

    void insertItemBeforeInternal(const SP<Item>& item, const SP<const Item>& reference);
    void removeChildItemInternal(const SP<Item>& item);

    QString _name;
    Location _location;
    WP<Item> _parentItem;
    std::vector<SP<Item>> _childItems;
};

} // namespace Lattice
