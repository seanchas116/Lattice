#pragma once
#include "../../render/RenderableObject.hpp"

namespace Lattice {

namespace Document {
class Mesh;
}

namespace GL {
class VAO;
}

namespace Editor {
class MeshPicker;
namespace Manipulator {

class RotateHandle : public Render::RenderableObject, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    RotateHandle(int axis);

    void draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;
    void drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;

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

    int _axis;
    glm::dvec3 _targetPosition {0};
    SP<Document::Mesh> _handleMesh;
    SP<MeshPicker> _handleMeshPicker;
    SP<GL::VAO> _handleVAO;
    glm::dvec3 _initialTargetPosition {0};
};

}
} // namespace Editor
} // namespace Lattice
