#pragma once
#include <glm/glm.hpp>

namespace Lattice {
namespace Viewport {

class PickableID {
public:
    PickableID(int renderableID, int fragmentID = 0) : _renderableID(renderableID), _fragmentID(fragmentID) {}

    PickableID withFragmentID(int fragmentID) const {
        return PickableID(_renderableID, fragmentID);
    }

    glm::vec4 toColor() const {
        union {
            struct {
                int renderableID;
                int fragmentID;
            };
            glm::u16vec4 color;
        } data;
        data.renderableID = _renderableID;
        data.fragmentID = _fragmentID;
        return data.color;
    }

private:
    int _renderableID;
    int _fragmentID;
};

} // namespace Viewport
} // namespace Lattice
