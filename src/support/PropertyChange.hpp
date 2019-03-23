#pragma once
#include "Change.hpp"
#include "Shorthands.hpp"

namespace Lattice {

template <typename TObject, typename TValue, typename TGetter, typename TSetter>
class PropertyChange : public Change {
public:
    PropertyChange(const std::unordered_map<SP<TObject>, TValue>& values, TGetter getter, TSetter setter) :
        newValues(values), getter(getter), setter(setter)
    {
    }

    void apply() override {
        std::vector<SP<TObject>> objects;
        objects.reserve(newValues.size());

        for (auto& [obj, value] : newValues) {
            oldValues[obj] = getter(obj);
            setter(obj, value);
            objects.push_back(obj);
        }
    }

    SP<Change> invert() const override {
        return makeShared<PropertyChange>(oldValues, getter, setter);
    }

    bool mergeWith(const SP<const Change>& other) override {
        LATTICE_OPTIONAL_GUARD(change, dynamicPointerCast<const PropertyChange>(other), return false;)

        auto oldValuesMerged = change->oldValues;
        for (auto& [obj, value] : oldValues) {
            oldValuesMerged[obj] = value;
        }
        oldValues = oldValuesMerged;

        for (auto& [obj, value] : change->newValues) {
            newValues[obj] = value;
        }
        return true;
    }

    std::unordered_map<SP<TObject>, TValue> oldValues;
    std::unordered_map<SP<TObject>, TValue> newValues;
    TGetter getter;
    TSetter setter;
};

template <typename TObject, typename TValue, typename TGetter, typename TSetter>
auto makePropertyChange(const std::unordered_map<SP<TObject>, TValue>& positions, TGetter getter, TSetter setter) {
    return makeShared<PropertyChange<TObject, TValue, TGetter, TSetter>>(positions, getter, setter);
}

} // namespace Lattice
