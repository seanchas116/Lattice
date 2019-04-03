#pragma once

#include <QObject>
#include <unordered_set>
#include <optional>
#include "../support/Shorthands.hpp"
#include "../mesh/MeshFragment.hpp"

namespace Lattice {
namespace Document {

class Object;
class MeshObject;
class History;

class Document final : public QObject, public EnableSharedFromThis<Document> {
    Q_OBJECT
public:
    Document();

    auto& rootObject() const { return _rootObject; }

    auto& currentObject() const { return _currentObject; }
    void setCurrentObject(const Opt<SP<Object>>& item);

    auto& editedObject() const { return _editedObject; }
    void setEditedObject(const Opt<SP<MeshObject>>& item);

    bool isEditing() const { return bool(_editedObject); }
    void setIsEditing(bool isEditing);

    auto& selectedObjects() const { return _selectedObjects; }
    void setSelectedObjects(const std::unordered_set<SP<Object>>& items);

    void selectObject(const SP<Object>& item, bool append);

    void insertObjectToCurrentPosition(const SP<Object>& item);
    void deleteSelectedObjects();

    auto& history() const { return _history; }

    auto& meshSelection() const { return _meshSelection; }
    void setMeshSelection(const Mesh::MeshFragment &meshSelection);

signals:
    void currentObjectChanged(const Opt<SP<Object>>& item);
    void editedObjectChanged(const Opt<SP<MeshObject>>& item);
    void isEditingChanged(bool isEditing);
    void selectedObjectsChanged(const std::unordered_set<SP<Object>>& items);
    void meshSelectionChanged(const Mesh::MeshFragment &meshSelection);

    void objectAboutToBeInserted(const SP<Object>& item);
    void objectInserted(const SP<Object>& item);
    void objectAboutToBeRemoved(const SP<Object>& item);
    void objectRemoved(const SP<Object>& item);

private:
    void watchChildrenInsertRemove(const SP<Object>& item);

    SP<Object> _rootObject;

    Opt<SP<Object>> _currentObject;
    Opt<SP<MeshObject>> _editedObject;
    std::unordered_set<SP<Object>> _selectedObjects;
    Mesh::MeshFragment _meshSelection;

    SP<History> _history;
};

}
} // namespace Lattice
