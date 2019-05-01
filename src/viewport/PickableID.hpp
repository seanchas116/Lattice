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

    int _renderableID;
    int _fragmentID;

public:
    PickableID(int renderableID, int fragmentID = 0) : _renderableID(renderableID), _fragmentID(fragmentID) {}

    PickableID withFragmentID(int fragmentID) const {
        return PickableID(_renderableID, fragmentID);
    }

    glm::vec4 toColor() const {
        Data data;
        data.renderableID = _renderableID;
        data.fragmentID = _fragmentID;
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
