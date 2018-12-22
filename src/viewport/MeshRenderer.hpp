#pragma once

#include "../support/Pointer.hpp"
#include <glm/glm.hpp>

namespace Lattice {

class VAO;
class LineVAO;
class PointVAO;
class MeshShape;
class Shaders;
class Projection;

class MeshRenderer {
public:
    MeshRenderer();
    void update(const SP<MeshShape>& shape);

    void drawFaces();
    void drawEdges(const SP<Shaders>& shaders, const glm::mat4& viewMatrix, const Projection& projection);
    void drawVertices(const SP<Shaders>& shaders, const glm::mat4& viewMatrix, const Projection& projection);

private:
    SP<VAO> _faceVAO;
    SP<LineVAO> _edgeVAO;
    SP<PointVAO> _vertexVAO;
};

}
