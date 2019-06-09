#pragma once

namespace Lattice {

class Camera {
public:
    enum class Projection {
        Perspective,
        Orthographic,
    };

    Camera();
};

} // namespace Lattice
