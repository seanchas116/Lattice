#pragma once

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

class QString;

namespace Lattice::impl {

template<typename T, std::size_t... I> void to_json(nlohmann::json& json, const T& a, std::index_sequence<I...>) {
    json = {a[I]...};
}

template<typename T, std::size_t... I> void from_json(const nlohmann::json& json, T& a, std::index_sequence<I...>) {
    a = {json[I]...};
}

}

namespace glm {

template<length_t L, typename T, qualifier Q, typename Indices = std::make_index_sequence<L>> void to_json(nlohmann::json& json, vec<L, T, Q> v) {
    Lattice::impl::to_json(json, v, Indices{});
}

template<length_t L, typename T, qualifier Q, typename Indices = std::make_index_sequence<L>> void from_json(const nlohmann::json& json, vec<L, T, Q> &v) {
    Lattice::impl::from_json(json, v, Indices{});
}

}

void to_json(nlohmann::json& json, const QString& string);
void from_json(const nlohmann::json& json, QString& string);
