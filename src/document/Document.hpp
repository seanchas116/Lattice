#pragma once

#include <QObject>
#include "../support/Pointer.hpp"

namespace Lattice {

class Item;
class History;

class Document final : public QObject, public std::enable_shared_from_this<Document> {
    Q_OBJECT
public:
    Document();

    void addInitialItems();

    const SP<Item>& rootItem() const { return _rootItem; }

    const std::vector<SP<Item>>& selectedItems() const { return _selectedItems; }
    void setSelectedItems(const std::vector<SP<Item>>& items);

    void insertItemToCurrentPosition(const SP<Item>& item);
    void deleteSelectedItems();

    const SP<History>& history() const { return _history; }

signals:
    void selectedItemsChanged(const std::vector<SP<Item>>& items);

    void itemInserted(const SP<Item>& item);
    void itemAboutToBeRemoved(const SP<Item>& item);

private:
    void watchChildrenInsertRemove(const SP<Item>& item);

    SP<Item> _rootItem;
    std::vector<SP<Item>> _selectedItems;
    SP<History> _history;
};

} // namespace Lattice
