#pragma once

#include "../support/Pointer.hpp"
#include "../support/Box.hpp"
#include <glm/glm.hpp>
#include <unordered_map>

namespace Lattice {
class Camera;
}

namespace Lattice::Document {
class MeshItem;
class Mesh;
class MeshMaterial;
}

namespace Lattice::GL {
class VAO;
class LineVAO;
class PointVAO;
}

namespace Lattice::Viewport {

class Operations;

class MeshRenderer final {
public:
    MeshRenderer(const SP<Document::MeshItem>& item);

    void drawFaces(const SP<Operations>& operations, const Camera& camera);
    void drawEdges(const SP<Operations> &operations, const Camera &camera);
    void drawVertices(const SP<Operations>& operations, const Camera& camera);

private:
    void updateVAOs(const SP<Document::Mesh>& mesh);
    void updateBoundingBox();

    SP<Document::MeshItem> _item;
    std::unordered_map<SP<Document::MeshMaterial>, SP<GL::VAO>> _faceVAOs;
    SP<GL::LineVAO> _edgeVAO;
    SP<GL::PointVAO> _vertexVAO;
    Box<float> _boundingBox;
};

}
