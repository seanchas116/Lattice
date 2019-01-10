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

namespace Lattice::UI {
class AppState;
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

namespace Lattice::Editor {

class Operations;

class MeshRenderer final : public Render::Renderable {
public:
    MeshRenderer(const SP<UI::AppState>& appState, const SP<Document::MeshItem>& item);

    void draw(const SP<Render::Operations> &operations, const Camera &camera) override;

private:
    void updateVAOs(const SP<Document::Mesh>& mesh);

    SP<UI::AppState> _appState;
    SP<Document::MeshItem> _item;
    std::unordered_map<SP<Document::MeshMaterial>, SP<GL::VAO>> _faceVAOs;
    SP<GL::LineVAO> _edgeVAO;
    SP<GL::PointVAO> _vertexVAO;
};

}
