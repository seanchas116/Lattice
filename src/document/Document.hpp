#pragma once

#include <QObject>
#include <unordered_set>
#include <optional>
#include "../support/Pointer.hpp"
#include "MeshSelection.hpp"

namespace Lattice::Document {

class Item;
class MeshItem;
class History;

class Document final : public QObject, public EnableSharedFromThis<Document> {
    Q_OBJECT
public:
    Document();

    const SP<Item>& rootItem() const { return _rootItem; }

    auto& currentItem() const { return _currentItem; }
    void setCurrentItem(const std::optional<SP<Item>>& item);

    auto& editedItem() const { return _editedItem; }
    void setEditedItem(const std::optional<SP<MeshItem>>& item);

    bool isEditing() const { return bool(_editedItem); }
    void setIsEditing(bool isEditing);

    auto& selectedItems() const { return _selectedItems; }
    void setSelectedItems(const std::unordered_set<SP<Item>>& items);

    void selectItem(const SP<Item>& item, bool append);

    void insertItemToCurrentPosition(const SP<Item>& item);
    void deleteSelectedItems();

    const SP<History>& history() const { return _history; }

    auto& meshSelection() const { return _meshSelection; }
    void setMeshSelection(const MeshSelection &meshSelection);

signals:
    void currentItemChanged(const std::optional<SP<Item>>& item);
    void editedItemChanged(const std::optional<SP<Item>>& item);
    void isEditingChanged(bool isEditing);
    void selectedItemsChanged(const std::unordered_set<SP<Item>>& items);
    void meshSelectionChanged(const MeshSelection &meshSelection);

    void itemInserted(const SP<Item>& item);
    void itemAboutToBeRemoved(const SP<Item>& item);

private:
    void watchChildrenInsertRemove(const SP<Item>& item);

    SP<Item> _rootItem;

    std::optional<SP<Item>> _currentItem;
    std::optional<SP<MeshItem>> _editedItem;
    std::unordered_set<SP<Item>> _selectedItems;
    MeshSelection _meshSelection;

    SP<History> _history;
};

} // namespace Lattice
