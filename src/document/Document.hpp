#pragma once

#include <QObject>
#include <unordered_set>
#include <optional>
#include "../support/Shorthands.hpp"

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

    auto& selectedObjects() const { return _selectedObjects; }
    void setSelectedObjects(const std::unordered_set<SP<Object>>& objects);

    void selectObject(const SP<Object>& object, bool append);

    void insertObjectToCurrentPosition(const SP<Object>& object);
    void deleteSelectedObjects();

    auto& history() const { return _history; }

signals:
    void currentObjectChanged(const Opt<SP<Object>>& object);
    void selectedObjectsChanged(const std::unordered_set<SP<Object>>& objects);

    void objectAboutToBeInserted(const SP<Object>& object);
    void objectInserted(const SP<Object>& object);
    void objectAboutToBeRemoved(const SP<Object>& object);
    void objectRemoved(const SP<Object>& object);

private:
    void watchChildrenInsertRemove(const SP<Object>& object);

    SP<Object> _rootObject;

    Opt<SP<Object>> _currentObject;
    std::unordered_set<SP<Object>> _selectedObjects;

    SP<History> _history;
};

}
} // namespace Lattice
