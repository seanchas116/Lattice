#pragma once

#include "../support/Pointer.hpp"
#include <glm/glm.hpp>

namespace Lattice {

class VAO;
class LineVAO;
class PointVAO;
class Mesh;
class Operations;
class Projection;

class MeshRenderer {
public:
    MeshRenderer();
    void update(const SP<Mesh>& shape);

    void drawFaces(const SP<Operations>& operations, const glm::mat4& viewMatrix, const Projection& projection);
    void drawEdges(const SP<Operations> &operations, const glm::mat4 &viewMatrix, const Projection &projection);
    void drawVertices(const SP<Operations>& operations, const glm::mat4& viewMatrix, const Projection& projection);

private:
    SP<VAO> _faceVAO;
    SP<LineVAO> _edgeVAO;
    SP<PointVAO> _vertexVAO;
};

}
