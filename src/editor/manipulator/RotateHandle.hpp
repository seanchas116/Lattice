#pragma once
#include "../../render/Renderable.hpp"

namespace Lattice {

namespace Document {
class Mesh;
}

namespace GL {
class OldVAO;
class LineVAO;
}

namespace Editor {
class MeshPicker;
namespace Manipulator {

class RotateHandle : public Render::Renderable, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    RotateHandle(int axis);

    void draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;

    std::optional<Render::HitResult> hitTest(glm::dvec2 pos, const SP<Camera> &camera) const override;

    void mousePress(const Render::MouseEvent &event) override;
    void mouseMove(const Render::MouseEvent &event) override;
    void mouseRelease(const Render::MouseEvent &event) override;

    void setTargetPosition(const glm::dvec3 &targetPosition) { _targetPosition = targetPosition; }

signals:
    void onBegin(double value);
    void onChange(double value);
    void onEnd();

private:
    SP<Document::Mesh> createMesh();
    SP<GL::LineVAO> createHandleVAO();

    int _axis;
    glm::dvec3 _targetPosition {0};
    SP<Document::Mesh> _handleMesh;
    SP<MeshPicker> _handleMeshPicker;
    SP<GL::LineVAO> _handleVAO;
    glm::dvec3 _initialTargetPosition {0};
};

}
} // namespace Editor
} // namespace Lattice
