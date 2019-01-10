#pragma once

#include "../support/Pointer.hpp"
#include "../support/Box.hpp"
#include "../support/Location.hpp"
#include "../render/Renderable.hpp"
#include <glm/glm.hpp>
#include <unordered_map>

class QMouseEvent;

namespace Lattice {

class Camera;

namespace UI {
class AppState;
}

namespace Document {
class MeshItem;
class Mesh;
class MeshMaterial;
}

namespace GL {
class VAO;
class LineVAO;
class PointVAO;
}

namespace Editor {

class MeshPicker;

class MeshRenderer final : public QObject, public Render::Renderable {
    Q_OBJECT
public:
    MeshRenderer(const SP<UI::AppState>& appState, const SP<Document::MeshItem>& item);

    void draw(const SP<Render::Operations> &operations, const Camera &camera) override;

    std::pair<bool, double> mousePress(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) override;
    void mouseMove(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) override;
    void mouseRelease(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) override;

private:
    void updateVAOs(const SP<Document::Mesh>& mesh);

    SP<UI::AppState> _appState;
    SP<Document::MeshItem> _item;
    SP<MeshPicker> _meshPicker;
    std::unordered_map<SP<Document::MeshMaterial>, SP<GL::VAO>> _faceVAOs;
    SP<GL::LineVAO> _edgeVAO;
    SP<GL::PointVAO> _vertexVAO;

    Location _dragInitLocation;
    glm::dvec3 _dragInitWorldPos;
    double _dragInitDepth;
    bool _dragStarted;
};

}
}
