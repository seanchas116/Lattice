#pragma once

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

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

template<length_t L, typename T, qualifier Q> void to_json(nlohmann::json& json, vec<L, T, Q> v) {
    Lattice::impl::to_json(json, v, std::make_index_sequence<L>{});
}

template<length_t L, typename T, qualifier Q> void from_json(const nlohmann::json& json, vec<L, T, Q> &v) {
    Lattice::impl::from_json(json, v, std::make_index_sequence<L>{});
}

template<typename T> void to_json(nlohmann::json& json, tquat<T> v) {
    json = {v.x, v.y, v.z, v.w};
}

template<typename T> void from_json(const nlohmann::json& json, tquat<T>& v) {
    v = {json[0], json[1], json[2], json[3]};
}

}

void to_json(nlohmann::json& json, const QString& string);
void from_json(const nlohmann::json& json, QString& string);
