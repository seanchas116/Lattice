#pragma once
#include <unordered_map>
#include <any>
#include <typeindex>

namespace Lattice {

class SharedResourceBag {
public:
    SharedResourceBag();

    template <typename T>
    T getOrCreate(T (*factory)()) {
        auto it = instances.find(factory);
        if (it == instances.end()) {
            it = instances.insert({factory, factory()});
        }
        return std::any_cast<T>(it->second);
    }

private:
    std::unordered_map<void*, std::any> instances;
};

} // namespace Lattice
