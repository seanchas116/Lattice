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
    void setCurrentObject(const Opt<SP<Object>>& object);

    auto& editedObject() const { return _editedObject; }
    void setEditedObject(const Opt<SP<MeshObject>>& object);

    bool isEditing() const { return bool(_editedObject); }
    void setIsEditing(bool isEditing);

    auto& selectedObjects() const { return _selectedObjects; }
    void setSelectedObjects(const std::unordered_set<SP<Object>>& objects);

    void selectObject(const SP<Object>& object, bool append);

    void insertObjectToCurrentPosition(const SP<Object>& object);
    void deleteSelectedObjects();

    auto& history() const { return _history; }

    auto& meshSelection() const { return _meshSelection; }
    void setMeshSelection(const OldMesh::MeshFragment &meshSelection);

signals:
    void currentObjectChanged(const Opt<SP<Object>>& object);
    void editedObjectChanged(const Opt<SP<MeshObject>>& object);
    void isEditingChanged(bool isEditing);
    void selectedObjectsChanged(const std::unordered_set<SP<Object>>& objects);
    void meshSelectionChanged(const OldMesh::MeshFragment &meshSelection);

    void objectAboutToBeInserted(const SP<Object>& object);
    void objectInserted(const SP<Object>& object);
    void objectAboutToBeRemoved(const SP<Object>& object);
    void objectRemoved(const SP<Object>& object);

private:
    void watchChildrenInsertRemove(const SP<Object>& object);

    SP<Object> _rootObject;

    Opt<SP<Object>> _currentObject;
    Opt<SP<MeshObject>> _editedObject;
    std::unordered_set<SP<Object>> _selectedObjects;
    OldMesh::MeshFragment _meshSelection;

    SP<History> _history;
};

}
} // namespace Lattice
