#include "Location.hpp"
#include "JSON.hpp"
#include <nlohmann/json.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace glm;

namespace Lattice {

glm::mat4 Location::matrix() const {
    return glm::translate(mat4(1), position) * glm::yawPitchRoll(rotation.y, rotation.x, rotation.z) * glm::scale(mat4(1), scale);
}

glm::vec3 Location::right() const {
    return matrix()[0].xyz;
}

glm::vec3 Location::up() const {
    return matrix()[1].xyz;
}

glm::vec3 Location::backward() const {
    return matrix()[2].xyz;
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
