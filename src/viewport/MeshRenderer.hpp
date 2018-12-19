#pragma once

#include "../support/Pointer.hpp"

namespace Lattice {

class VAO;
class LineVAO;
class PointVAO;
class MeshShape;

class MeshRenderer {
public:
    MeshRenderer();
    void update(const SP<MeshShape>& shape);

    void drawFaces();
    void drawEdges();
    void drawVertices();

private:
    SP<VAO> _faceVAO;
    SP<LineVAO> _edgeVAO;
    SP<PointVAO> _vertexVAO;
};

}
