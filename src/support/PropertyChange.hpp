#pragma once
#include "Change.hpp"
#include "Shorthands.hpp"

namespace Lattice {

template <typename TObject, typename TValue, typename TGetter, typename TSetter>
class PropertyChange : public Change {
public:
    PropertyChange(const std::unordered_map<SP<TObject>, TValue>& values, TGetter getter, TSetter setter) :
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
    std::unordered_map<SP<TObject>, TValue> _oldValues;
    std::unordered_map<SP<TObject>, TValue> _newValues;
    TGetter _getter;
    TSetter _setter;
};

template <typename TObject, typename TValue, typename TGetter, typename TSetter>
auto makePropertyChange(const std::unordered_map<SP<TObject>, TValue>& positions, TGetter getter, TSetter setter) {
    return makeShared<PropertyChange<TObject, TValue, TGetter, TSetter>>(positions, getter, setter);
}

} // namespace Lattice
