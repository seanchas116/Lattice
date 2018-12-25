#pragma once

#include <QObject>
#include <unordered_set>
#include "../support/Pointer.hpp"

namespace Lattice {

class Item;
class MeshItem;
class History;

class Document final : public QObject, public std::enable_shared_from_this<Document> {
    Q_OBJECT
public:
    Document();

    void addInitialItems();

    const SP<Item>& rootItem() const { return _rootItem; }

    const SP<Item>& currentItem() const { return _currentItem; }
    SP<MeshItem> currentMeshItem() const;
    void setCurrentItem(const SP<Item>& item);

    auto& selectedItems() const { return _selectedItems; }
    void setSelectedItems(const std::unordered_set<SP<Item>>& items);

    void insertItemToCurrentPosition(const SP<Item>& item);
    void deleteSelectedItems();

    const SP<History>& history() const { return _history; }

signals:
    void currentItemChanged(const SP<Item>& item);
    void currentMeshItemChanged(const SP<Item>& item);
    void selectedItemsChanged(const std::unordered_set<SP<Item>>& items);

    void itemInserted(const SP<Item>& item);
    void itemAboutToBeRemoved(const SP<Item>& item);

private:
    void watchChildrenInsertRemove(const SP<Item>& item);

    SP<Item> _rootItem;

    SP<Item> _currentItem;
    std::unordered_set<SP<Item>> _selectedItems;

    SP<History> _history;
};

} // namespace Lattice
