#pragma once
#include <glm/glm.hpp>

namespace Lattice {
namespace Viewport {

class PickableID {
    struct Data {
        struct {
            int renderableID;
            int fragmentID;
        };
        glm::u16vec4 color;
    };

public:
    int renderableID;
    int fragmentID;

    PickableID(int renderableID, int fragmentID = 0) : renderableID(renderableID), fragmentID(fragmentID) {}

    PickableID withFragmentID(int fragmentID) const {
        return PickableID(renderableID, fragmentID);
    }

    glm::vec4 toColor() const {
        Data data;
        data.renderableID = renderableID;
        data.fragmentID = fragmentID;
        return data.color;
    }

    static PickableID fromIDColor(glm::vec4 color) {
        Data data;
        data.color = color;
        return PickableID(data.renderableID, data.fragmentID);
    }
};

} // namespace Viewport
} // namespace Lattice
