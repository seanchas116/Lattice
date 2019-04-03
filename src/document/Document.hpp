#pragma once

#include <QObject>
#include <unordered_set>
#include <optional>
#include "../support/Shorthands.hpp"
#include "../mesh/MeshFragment.hpp"

namespace Lattice {
namespace Document {

class Object;
class MeshItem;
class History;

class Document final : public QObject, public EnableSharedFromThis<Document> {
    Q_OBJECT
public:
    Document();

    const SP<Object>& rootItem() const { return _rootItem; }

    auto& currentItem() const { return _currentItem; }
    void setCurrentItem(const Opt<SP<Object>>& item);

    auto& editedItem() const { return _editedItem; }
    void setEditedItem(const Opt<SP<MeshItem>>& item);

    bool isEditing() const { return bool(_editedItem); }
    void setIsEditing(bool isEditing);

    auto& selectedItems() const { return _selectedItems; }
    void setSelectedItems(const std::unordered_set<SP<Object>>& items);

    void selectItem(const SP<Object>& item, bool append);

    void insertItemToCurrentPosition(const SP<Object>& item);
    void deleteSelectedItems();

    const SP<History>& history() const { return _history; }

    auto& meshSelection() const { return _meshSelection; }
    void setMeshSelection(const Mesh::MeshFragment &meshSelection);

signals:
    void currentItemChanged(const Opt<SP<Object>>& item);
    void editedItemChanged(const Opt<SP<MeshItem>>& item);
    void isEditingChanged(bool isEditing);
    void selectedItemsChanged(const std::unordered_set<SP<Object>>& items);
    void meshSelectionChanged(const Mesh::MeshFragment &meshSelection);

    void itemAboutToBeInserted(const SP<Object>& item);
    void itemInserted(const SP<Object>& item);
    void itemAboutToBeRemoved(const SP<Object>& item);
    void itemRemoved(const SP<Object>& item);

private:
    void watchChildrenInsertRemove(const SP<Object>& item);

    SP<Object> _rootItem;

    Opt<SP<Object>> _currentItem;
    Opt<SP<MeshItem>> _editedItem;
    std::unordered_set<SP<Object>> _selectedItems;
    Mesh::MeshFragment _meshSelection;

    SP<History> _history;
};

}
} // namespace Lattice
