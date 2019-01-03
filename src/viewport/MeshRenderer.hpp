#pragma once

#include "../support/Pointer.hpp"
#include <glm/glm.hpp>
#include <unordered_map>

namespace Lattice {
class MeshItem;
class Mesh;
class Camera;
class MeshMaterial;
}

namespace Lattice::GL {
class VertexBuffer;
class VAO;
class LineVAO;
class PointVAO;
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
    SP<GL::VertexBuffer> _vbo;
    std::unordered_map<SP<MeshMaterial>, SP<GL::VAO>> _faceVAOs;
    SP<GL::LineVAO> _edgeVAO;
    SP<GL::PointVAO> _vertexVAO;
};

}
