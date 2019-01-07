#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <nlohmann/json_fwd.hpp>

namespace Lattice {

class Location final {
public:
    glm::dvec3 scale {1};
    glm::dquat rotation {1, 0, 0, 0};
    glm::dvec3 position {0};

    glm::dmat4 matrix() const;

    glm::dvec3 right() const;
    glm::dvec3 up() const;
    glm::dvec3 backward() const;

    bool operator==(const Location& other) const;
    bool operator!=(const Location& other) const;
};

void to_json(nlohmann::json& json, const Location& location);
void from_json(const nlohmann::json& json, Location& location);

} // namespace Lattice
