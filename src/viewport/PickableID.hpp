#pragma once
#include <glm/glm.hpp>

namespace Lattice {
namespace Viewport {

class PickableID {
    struct Data {
        struct {
            int32_t renderableID;
            int32_t fragmentID;
        };
        glm::u16vec4 color;
    };

public:
    int32_t renderableID;
    int32_t fragmentID;

    PickableID(int32_t renderableID, int32_t fragmentID = 0) : renderableID(renderableID), fragmentID(fragmentID) {}

    PickableID withFragmentID(int32_t fragmentID) const {
        return PickableID(renderableID, fragmentID);
    }

    glm::vec4 toColor() const {
        Data data;
        data.renderableID = renderableID;
        data.fragmentID = fragmentID;
        return glm::vec4(data.color) / float(0xFFFF);
    }

    static PickableID fromIDColor(glm::vec4 color) {
        Data data;
        data.color = glm::u16vec4(glm::round(color * float(0xFFFF)));
        return PickableID(data.renderableID, data.fragmentID);
    }
};

} // namespace Viewport
} // namespace Lattice
