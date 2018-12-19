#pragma once

#include "../support/Pointer.hpp"

namespace Lattice {

class VAO;
class LineVAO;
class MeshShape;

class MeshRenderer {
public:
    MeshRenderer();
    void update(const SP<MeshShape>& shape);

    void drawFaces();
    void drawEdges();

private:
    SP<VAO> _faceVAO;
    SP<LineVAO> _edgeVAO;
};

}
