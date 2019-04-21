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

class Object : public QObject, public EnableSharedFromThis<Object> {
    Q_OBJECT
public:
    Opt<SP<Object>> parentObject() const;
    const std::vector<SP<Object>>& childObjects() const { return _childObjects; }
    Opt<SP<Object>> nextObject() const;
    void appendChildObject(const SP<Object>& object);
    void insertObjectBefore(const SP<Object>& object, const Opt<SP<const Object> > &reference);
    void removeChildObject(const SP<Object>& object);
    int index() const;
    std::vector<int> indexPath() const;

    auto& name() const { return _name; }
    void setName(const std::string& name);

    auto& location() const { return _location; }
    void setLocation(const Location &location);

    void forEachDescendant(const Fn<void(const SP<Object> &)> &callback);

    virtual bool canInsertObject(const SP<const Object>& object) const;
    virtual SP<Object> clone() const = 0;
    virtual void toJSON(nlohmann::json& json) const;
    virtual void fromJSON(const nlohmann::json& json);
    virtual Opt<SP<Document>> document() const;

protected:
    void addChange(const SP<Change>& change);

signals:
    void nameChanged(const std::string& name);
    void locationChanged(const Location &location);

    void childObjectsAboutToBeInserted(int first, int last, const std::vector<SP<Object>>& objects);
    void childObjectsInserted(int first, int last);
    void childObjectsAboutToBeRemoved(int first, int last);
    void childObjectsRemoved(int first, int last, const std::vector<SP<Object>>& objects);

private:
    class NameChange;
    class LocationChange;
    class ChildInsertChange;
    class ChildRemoveChange;

    void setNameInternal(const std::string& name);
    void setLocationInternal(const Location& location);

    void insertObjectBeforeInternal(const SP<Object>& object, const Opt<SP<const Object> > &reference);
    void removeChildObjectInternal(const SP<Object>& object);

    std::string _name;
    Location _location;
    WP<Object> _parentObject;
    std::vector<SP<Object>> _childObjects;
};

}
}
