#pragma once

#include "../support/Pointer.hpp"
#include <glm/glm.hpp>

namespace Lattice {

class MeshItem;
class VAO;
class LineVAO;
class PointVAO;
class Mesh;
class Operations;
class Projection;

class MeshRenderer {
public:
    MeshRenderer(const SP<MeshItem>& item);

    void drawFaces(const SP<Operations>& operations, const glm::mat4& viewMatrix, const Projection& projection);
    void drawEdges(const SP<Operations> &operations, const glm::mat4 &viewMatrix, const Projection &projection);
    void drawVertices(const SP<Operations>& operations, const glm::mat4& viewMatrix, const Projection& projection);

private:
    void update(const SP<Mesh>& mesh);

    SP<MeshItem> _item;
    SP<VAO> _faceVAO;
    SP<LineVAO> _edgeVAO;
    SP<PointVAO> _vertexVAO;
};

}
