#pragma once
#include <array>
#include <set>
#include <tuple>
#include <vector>

namespace std {
namespace {
// https://stackoverflow.com/a/7115547
template <class T>
inline void hash_combine(std::size_t &seed, T const &v) {
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// Recursive template code derived from Matthieu M.
template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
struct HashValueImpl {
    static void apply(size_t &seed, Tuple const &tuple) {
        HashValueImpl<Tuple, Index - 1>::apply(seed, tuple);
        hash_combine(seed, std::get<Index>(tuple));
    }
};

template <class Tuple>
struct HashValueImpl<Tuple, 0> {
    static void apply(size_t &seed, Tuple const &tuple) {
        hash_combine(seed, std::get<0>(tuple));
    }
};
} // namespace

template <typename... TT>
struct hash<std::tuple<TT...>> {
    size_t operator()(std::tuple<TT...> const &tt) const {
        size_t seed = 0;
        HashValueImpl<std::tuple<TT...>>::apply(seed, tt);
        return seed;
    }
};

template <typename T, typename U>
struct hash<std::pair<T, U>> {
    size_t operator()(std::pair<T, U> const &pair) const {
        size_t seed = 0;
        hash_combine(seed, pair.first);
        hash_combine(seed, pair.second);
        return seed;
    }
};

template <typename T, size_t N>
struct hash<std::array<T, N>> {
    size_t operator()(std::array<T, N> const &xs) const {
        size_t seed = 0;
        HashValueImpl<std::array<T, N>>::apply(seed, xs);
        return seed;
    }
};

template <typename T>
struct hash<std::vector<T>> {
    size_t operator()(std::vector<T> const &xs) const {
        size_t seed = 0;
        for (auto &x : xs) {
            hash_combine(seed, x);
        }
        return seed;
    }
};

template <typename T>
struct hash<std::set<T>> {
    size_t operator()(std::set<T> const &xs) const {
        size_t seed = 0;
        for (auto &x : xs) {
            hash_combine(seed, x);
        }
        return seed;
    }
};
} // namespace std
