#pragma once
#include <QObject>
#include "../../render/Renderable.hpp"

namespace Lattice {

namespace Document {
class Mesh;
}

namespace GL {
class VAO;
class LineVAO;
}

namespace Editor {
class MeshPicker;
namespace Manipulator {

class RotateHandle : public QObject, public Render::Renderable, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    RotateHandle(int axis);

    void draw(const SP<Render::Operations> &operations, const Camera &camera) override;
    std::pair<bool, double> mousePress(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) override;
    void mouseMove(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) override;
    void mouseRelease(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) override;

    void setTargetPosition(const glm::dvec3 &targetPosition) { _targetPosition = targetPosition; }

signals:
    void rotateStarted();
    void rotateChanged(double offset);
    void rotateFinished();

private:
    SP<Document::Mesh> createMesh();
    SP<GL::LineVAO> createHandleVAO();

    int _axis;
    glm::dvec3 _targetPosition {0};
    SP<Document::Mesh> _handleMesh;
    SP<MeshPicker> _handleMeshPicker;
    SP<GL::LineVAO> _handleVAO;
    double _initialDragValue = 0;
    glm::dvec3 _initialTargetPosition {0};
};

}
} // namespace Editor
} // namespace Lattice
