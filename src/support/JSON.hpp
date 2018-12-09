#pragma once

#include <nlohmann/json_fwd.hpp>
#include <glm/glm.hpp>

class QString;

namespace glm {

void to_json(nlohmann::json &json, vec2 v);
void to_json(nlohmann::json &json, vec3 v);
void to_json(nlohmann::json &json, vec4 v);

void from_json(const nlohmann::json &json, vec2 &v);
void from_json(const nlohmann::json &json, vec3 &v);
void from_json(const nlohmann::json &json, vec4 &v);

}

void to_json(nlohmann::json& json, const QString& string);
void from_json(const nlohmann::json& json, QString& string);
