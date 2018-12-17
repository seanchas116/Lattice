#pragma once

#include "../support/Pointer.hpp"

namespace Lattice {

class VAO;
class LineVAO;

class MeshRenderer {
public:
    MeshRenderer();
private:
    SP<VAO> _faceVAO;
    SP<LineVAO> _edgeVAO;
};

}
