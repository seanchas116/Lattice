#pragma once

#include "../support/Pointer.hpp"

namespace Lattice {

class LineVAO;
class Shaders;
class Camera;
class Projection;

class GridFloor final {
public:
    GridFloor();

    void draw(const SP<Shaders>& shaders, const Camera& camera, const Projection& projection);

private:
    SP<LineVAO> _vao;
};

} // namespace Lattice
