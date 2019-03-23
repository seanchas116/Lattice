#pragma once
#include "Change.hpp"
#include "Shorthands.hpp"

namespace Lattice {

template <typename TObject, typename TValue>
class PropertyChange : public Change {
public:
    using Values = std::unordered_map<SP<TObject>, TValue>;
    using Getter = std::function<TValue(const SP<TObject>&)>;
    using Setter = std::function<void(const SP<TObject>&, TValue)>;

    PropertyChange(const Values& values, const Getter& getter, const Setter& setter) :
        _newValues(values), _getter(getter), _setter(setter)
    {
    }

    void apply() override {
        std::vector<SP<TObject>> objects;
        objects.reserve(_newValues.size());

        for (auto& [obj, value] : _newValues) {
            _oldValues[obj] = _getter(obj);
            _setter(obj, value);
            objects.push_back(obj);
        }
    }

    SP<Change> invert() const override {
        return makeShared<PropertyChange>(_oldValues, _getter, _setter);
    }

    bool mergeWith(const SP<const Change>& other) override {
        LATTICE_OPTIONAL_GUARD(change, dynamicPointerCast<const PropertyChange>(other), return false;)

        auto oldValuesMerged = change->_oldValues;
        for (auto& [obj, value] : _oldValues) {
            oldValuesMerged[obj] = value;
        }
        _oldValues = oldValuesMerged;

        for (auto& [obj, value] : change->_newValues) {
            _newValues[obj] = value;
        }
        return true;
    }

private:
    Values _oldValues;
    Values _newValues;
    Getter _getter;
    Setter _setter;
};

} // namespace Lattice
