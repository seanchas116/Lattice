#pragma once
#include <glm/glm.hpp>
#include <nlohmann/json_fwd.hpp>

namespace Lattice {

struct Location final {
    glm::vec3 position {0};
    glm::vec3 scale {1};
    glm::vec3 rotation {0};

    bool operator==(const Location& other) const;
    bool operator!=(const Location& other) const;
};

void to_json(nlohmann::json& json, const Location& location);
void from_json(const nlohmann::json& json, Location& location);

} // namespace Shapecraft
