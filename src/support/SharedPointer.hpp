#pragma once

#include <memory>
#include <optional>

namespace Lattice {

// wrapper of std::shared_ptr, but non nullable
template <typename T>
class SharedPointer {
  public:
    using element_type = T;

    template <typename U, typename... TArgs>
    friend SharedPointer<U> makeShared(TArgs &&... args);

    template <typename TOther,
              typename std::enable_if<std::is_convertible<std::shared_ptr<TOther>, std::shared_ptr<T>>::value, int>::type = 0>
    SharedPointer(const SharedPointer<TOther> &other) : _ptr(other.operator const std::shared_ptr<TOther> &()) {}

    template <typename TOther,
              typename std::enable_if<std::is_convertible<std::shared_ptr<TOther>, std::shared_ptr<T>>::value, int>::type = 0>
    SharedPointer(SharedPointer<TOther> &&other) : _ptr(std::move(other).operator std::shared_ptr<TOther> &&()) {}

    element_type &operator*() const { return *_ptr; }
    element_type *operator->() const { return _ptr.get(); }
    element_type *get() const { return _ptr.get(); }

    operator const std::shared_ptr<element_type> &() const { return _ptr; }
    operator std::shared_ptr<element_type> &&() && { return std::move(_ptr); }

  private:
    template <typename U>
    friend class WeakPointer;
    template <typename TDerived>
    friend class EnableSharedFromThis;

    template <typename A, typename B>
    friend bool operator==(const SharedPointer<A> &left, const SharedPointer<B> &right);

    template <typename A, typename B>
    friend bool operator!=(const SharedPointer<A> &left, const SharedPointer<B> &right);

    template <typename A, typename B>
    friend bool operator<(const SharedPointer<A> &left, const SharedPointer<B> &right);

    template <typename U, typename V>
    friend std::optional<SharedPointer<U>> dynamicPointerCast(const SharedPointer<V> &original);

    SharedPointer(const std::shared_ptr<element_type> &ptr) : _ptr(ptr) {}
    SharedPointer(std::shared_ptr<element_type> &&ptr) : _ptr(std::move(ptr)) {}

    std::shared_ptr<element_type> _ptr;
};

template <typename A, typename B>
bool operator==(const SharedPointer<A> &left, const SharedPointer<B> &right) {
    return left._ptr == right._ptr;
}

template <typename A, typename B>
bool operator!=(const SharedPointer<A> &left, const SharedPointer<B> &right) {
    return !(left == right);
}

template <typename A, typename B>
bool operator<(const SharedPointer<A> &left, const SharedPointer<B> &right) {
    return left._ptr < right._ptr;
}

template <typename U, typename... TArgs>
SharedPointer<U> makeShared(TArgs &&... args) {
    auto ptr = std::make_shared<U>(std::forward<TArgs>(args)...);
    return {ptr};
}

template <typename U, typename V>
std::optional<SharedPointer<U>> dynamicPointerCast(const SharedPointer<V> &original) {
    auto ptr = std::dynamic_pointer_cast<U>(original._ptr);
    if (ptr) {
        return {{ptr}};
    } else {
        return {};
    }
}

template <typename T, typename U>
std::optional<SharedPointer<T>> dynamicPointerCast(const std::optional<SharedPointer<U>> &original) {
    if (!original) {
        return {};
    }
    return dynamicPointerCast<T>(*original);
}

template <typename T>
class WeakPointer {
  public:
    using element_type = T;

    WeakPointer() {}

    template <typename TOther,
              typename std::enable_if<std::is_convertible<std::shared_ptr<TOther>, std::weak_ptr<T>>::value, int>::type = 0>
    WeakPointer(const SharedPointer<TOther> &other) : _ptr(other._ptr) {}

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
class EnableSharedFromThis : public std::enable_shared_from_this<TDerived> {
  public:
    SharedPointer<TDerived> sharedFromThis() {
        return {this->shared_from_this()};
    }
    SharedPointer<const TDerived> sharedFromThis() const {
        return {this->shared_from_this()};
    }
};

} // namespace Lattice

namespace std {

template <typename T>
struct hash<Lattice::SharedPointer<T>> {
    size_t operator()(const Lattice::SharedPointer<T> &value) const {
        return std::hash<T *>()(value.get());
    }
};

} // namespace std
