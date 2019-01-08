#pragma once

#include "../support/Pointer.hpp"
#include "../support/Box.hpp"
#include "../render/Renderable.hpp"
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

class MeshRenderer final : public Render::Renderable {
public:
    MeshRenderer(const SP<Document::MeshItem>& item);

    auto& facesRenderer() { return _facesRenderer; }
    auto& edgesRenderer() { return _edgesRenderer; }
    auto& verticesRenderer() { return _verticesRenderer; }

private:
    void updateVAOs(const SP<Document::Mesh>& mesh);

    SP<Document::MeshItem> _item;
    std::unordered_map<SP<Document::MeshMaterial>, SP<GL::VAO>> _faceVAOs;
    SP<GL::LineVAO> _edgeVAO;
    SP<GL::PointVAO> _vertexVAO;

    class FacesRenderer;
    class EdgesRenderer;
    class VerticesRenderer;

    SP<Render::Renderable> _facesRenderer;
    SP<Render::Renderable> _edgesRenderer;
    SP<Render::Renderable> _verticesRenderer;
};

}
