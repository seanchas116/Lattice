#include "JSON.hpp"
#include <QString>
#include <nlohmann/json.hpp>

namespace glm {

void to_json(nlohmann::json &json, vec2 v) {
    json = {v[0], v[1]};
}

void to_json(nlohmann::json &json, vec3 v) {
    json = {v[0], v[1], v[2]};
}

void to_json(nlohmann::json &json, vec4 v) {
    json = {v[0], v[1], v[2], v[3]};
}

void from_json(const nlohmann::json &json, vec2 &v) {
    v = {json[0], json[1]};
}

void from_json(const nlohmann::json &json, vec3 &v) {
    v = {json[0], json[1], json[2]};
}

void from_json(const nlohmann::json &json, vec4 &v) {
    v = {json[0], json[1], json[2], json[3]};
}

}

void to_json(nlohmann::json &json, const QString &string) {
    json = string.toStdString();
}

void from_json(const nlohmann::json &json, QString &string) {
    string = QString::fromStdString(json);
}
