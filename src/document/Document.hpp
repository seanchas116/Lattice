#pragma once

#include <QObject>
#include <unordered_set>
#include <optional>
#include "../support/Pointer.hpp"

namespace Lattice::Document {

class Item;
class MeshItem;
class History;

class Document final : public QObject, public EnableSharedFromThis<Document> {
    Q_OBJECT
public:
    Document();

    void addInitialItems();

    const SP<Item>& rootItem() const { return _rootItem; }

    const auto& currentItem() const { return _currentItem; }
    std::optional<SP<MeshItem>> currentMeshItem() const;
    void setCurrentItem(const std::optional<SP<Item>>& item);

    auto& selectedItems() const { return _selectedItems; }
    void setSelectedItems(const std::unordered_set<SP<Item>>& items);

    void selectItem(const SP<Item>& item, bool append);

    void insertItemToCurrentPosition(const SP<Item>& item);
    void deleteSelectedItems();

    const SP<History>& history() const { return _history; }

signals:
    void currentItemChanged(const std::optional<SP<Item>>& item);
    void currentMeshItemChanged(const std::optional<SP<MeshItem>>& item);
    void selectedItemsChanged(const std::unordered_set<SP<Item>>& items);

    void itemInserted(const SP<Item>& item);
    void itemAboutToBeRemoved(const SP<Item>& item);

private:
    void watchChildrenInsertRemove(const SP<Item>& item);

    SP<Item> _rootItem;

    std::optional<SP<Item>> _currentItem;
    std::unordered_set<SP<Item>> _selectedItems;

    SP<History> _history;
};

} // namespace Lattice
