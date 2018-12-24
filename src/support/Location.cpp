#include "Location.hpp"
#include "JSON.hpp"
#include <nlohmann/json.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Lattice {

glm::mat4 Location::matrix() const {
    glm::mat4 m(0);
    m = glm::scale(m, scale);
    m = glm::yawPitchRoll(rotation.y, rotation.x, rotation.z) * m;
    m = glm::translate(m, position);
    return m;
}

bool Location::operator==(const Location &other) const {
    return position == other.position && scale == other.scale && rotation == other.rotation;
}

bool Location::operator!=(const Location &other) const {
    return !operator==(other);
}

void to_json(nlohmann::json &json, const Location &location) {
    json["position"] = location.position;
    json["scale"] = location.scale;
    json["rotation"] = location.rotation;
}

void from_json(const nlohmann::json &json, Location &location) {
    location.position = json["position"];
    location.scale = json["scale"];
    location.rotation = json["rotation"];
}

} // namespace Lattice
