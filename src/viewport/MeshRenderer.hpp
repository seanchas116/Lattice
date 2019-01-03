#pragma once

#include "../support/Pointer.hpp"
#include <glm/glm.hpp>
#include <unordered_map>

namespace Lattice {

class MeshItem;
class VAO;
class LineVAO;
class PointVAO;
class Mesh;
class Camera;
class MeshMaterial;
class VertexBuffer;

}

namespace Lattice::Viewport {

class Operations;

class MeshRenderer final {
public:
    MeshRenderer(const SP<MeshItem>& item);

    void drawFaces(const SP<Operations>& operations, const Camera& camera);
    void drawEdges(const SP<Operations> &operations, const Camera &camera);
    void drawVertices(const SP<Operations>& operations, const Camera& camera);

private:
    void update(const SP<Mesh>& mesh);

    SP<MeshItem> _item;
    SP<VertexBuffer> _vbo;
    std::unordered_map<SP<MeshMaterial>, SP<VAO>> _faceVAOs;
    SP<LineVAO> _edgeVAO;
    SP<PointVAO> _vertexVAO;
};

}
