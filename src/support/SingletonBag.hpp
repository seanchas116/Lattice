#pragma once
#include <unordered_map>
#include <any>
#include <typeindex>

namespace Lattice {

class SingletonBag {
public:
    SingletonBag();

    template <typename T>
    T get(T (*factory)()) {
        auto key = reinterpret_cast<void *>(factory);
        auto it = instances.find(key);
        if (it == instances.end()) {
            bool inserted;
            std::tie(it, inserted) = instances.insert({key, factory()});
        }
        return std::any_cast<T>(it->second);
    }

private:
    std::unordered_map<void*, std::any> instances;
};

} // namespace Lattice
