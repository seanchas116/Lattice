#pragma once

#include <memory>
#include <optional>

namespace Lattice {

// wrapper of std::shared_ptr, but non nullable
template <typename T>
class SharedPointer {
public:
    using element_type = T;

    template <typename T, typename ...TArgs>
    friend SharedPointer<T> makeShared(TArgs&& ...args);

    template <typename TOther,
              typename std::enable_if<std::is_convertible<std::shared_ptr<TOther>, std::shared_ptr<T>>::value, int>::type = 0>
    SharedPointer(const SharedPointer<TOther>& other) : _ptr(other.operator const std::shared_ptr<TOther> &()) {}

    template <typename TOther,
              typename std::enable_if<std::is_convertible<std::shared_ptr<TOther>, std::shared_ptr<T>>::value, int>::type = 0>
    SharedPointer(SharedPointer<TOther>&& other) : _ptr(std::move(other).operator std::shared_ptr<TOther> &&()) {}

    template <typename A, typename B>
    friend bool operator==(const SharedPointer<A>& left, const SharedPointer<B>& right);

    template <typename A, typename B>
    friend bool operator!=(const SharedPointer<A>& left, const SharedPointer<B>& right);

    template <typename A, typename B>
    friend bool operator<(const SharedPointer<A>& left, const SharedPointer<B>& right);

    element_type& operator*() const { return *_ptr; }
    element_type* operator->() const { return _ptr.get(); }
    element_type* get() const { return _ptr.get(); }

    operator const std::shared_ptr<element_type>&() const { return _ptr; }
    operator std::shared_ptr<element_type>&&() &&{ return std::move(_ptr); }

private:
    template <typename U> friend class WeakPointer;
    SharedPointer(const std::shared_ptr<element_type>& ptr) : _ptr(ptr) {}
    SharedPointer(std::shared_ptr<element_type>&& ptr) : _ptr(std::move(ptr)) {}

    template <typename T, typename U>
    friend std::optional<SharedPointer<T>> dynamicPointerCast(const SharedPointer<U>& original);

    std::shared_ptr<element_type> _ptr;
};

template <typename A, typename B>
bool operator==(const SharedPointer<A>& left, const SharedPointer<B>& right) {
    return left._ptr == right._ptr;
}

template <typename A, typename B>
bool operator!=(const SharedPointer<A>& left, const SharedPointer<B>& right) {
    return !(left == right);
}

template <typename A, typename B>
bool operator<(const SharedPointer<A>& left, const SharedPointer<B>& right) {
    return left < right;
}

template <typename T>
class WeakPointer {
public:
    using element_type = T;

    WeakPointer() {}

    template <typename TOther,
              typename std::enable_if<std::is_convertible<std::shared_ptr<TOther>, std::weak_ptr<T>>::value, int>::type = 0>
    WeakPointer(const SharedPointer<TOther>& other) : _ptr(other._ptr) {}

    std::optional<SharedPointer<T>> lock() const {
        auto ptr = _ptr.lock();
        if (ptr) {
            return {SharedPointer{std::move(ptr)}};
        } else {
            return {};
        }
    }

    void reset() {
        _ptr.reset();
    }

private:
    std::weak_ptr<element_type> _ptr;
};

template <typename TDerived>
class EnableSharedFromThis : private std::enable_shared_from_this<TDerived> {
public:
    SharedPointer<TDerived> sharedFromThis();
    SharedPointer<const TDerived> sharedFromThis() const;
};

template <typename T, typename ...TArgs>
SharedPointer<T> makeShared(TArgs&& ...args) {
    auto ptr = std::make_shared<T>(std::forward<TArgs>(args)...);
    return {ptr};
}

template <typename T, typename U>
std::optional<SharedPointer<T>> dynamicPointerCast(const SharedPointer<U>& original) {
    auto ptr = std::dynamic_pointer_cast<T>(original._ptr);
    if (ptr) {
        return {{ptr}};
    } else {
        return {};
    }
}

} // namespace Lattice

namespace std {

template <typename T>
struct hash<Lattice::SharedPointer<T>> {
    size_t operator()(const Lattice::SharedPointer<T>& value) const {
        return std::hash<T*>()(value.get());
    }
};

}
