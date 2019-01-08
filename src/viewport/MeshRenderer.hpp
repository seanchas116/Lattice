#pragma once

#include "../support/Pointer.hpp"
#include "../support/Box.hpp"
#include "../renderer/Renderable.hpp"
#include <glm/glm.hpp>
#include <unordered_map>

class QMouseEvent;

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

    void drawFaces(const SP<Renderer::Operations>& operations, const Camera& camera);
    void drawEdges(const SP<Renderer::Operations> &operations, const Camera &camera);
    void drawVertices(const SP<Renderer::Operations>& operations, const Camera& camera);

private:
    void updateVAOs(const SP<Document::Mesh>& mesh);

    SP<Document::MeshItem> _item;
    std::unordered_map<SP<Document::MeshMaterial>, SP<GL::VAO>> _faceVAOs;
    SP<GL::LineVAO> _edgeVAO;
    SP<GL::PointVAO> _vertexVAO;
};

}
