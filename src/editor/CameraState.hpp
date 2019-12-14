#pragma once
#include "../support/Camera.hpp"
#include "../support/Property.hpp"
#include <QObject>
#include <glm/glm.hpp>

namespace Lattice {
namespace Editor {

class CameraState : public QObject {
    Q_OBJECT

  public:
    enum class Orientation {
        None,
        Front,
        Back,
        Right,
        Left,
        Top,
        Bottom,
    };

    CameraState();

    Camera camera() const;

    glm::dmat4 cameraToWorldMatrix() const;

    Orientation orientation() const;
    void setOrientation(Orientation orientation);

  signals:
    void cameraChanged(const Camera &camera);
    void orientationChanged(Orientation orientation);

  private:
    static glm::dvec3 orientationAngle(Orientation orientation);

    LATTICE_AUTO_PROPERTY_REFERENCE(Camera::Projection, projection, setProjection, Camera::Projection::Perspective)
    LATTICE_AUTO_PROPERTY(glm::dvec3, position, setPosition, glm::dvec3(0, 10, 10))
    LATTICE_AUTO_PROPERTY(glm::dvec3, eulerAngles, setEulerAngles, glm::dvec3(0))
    LATTICE_AUTO_PROPERTY(glm::dvec2, viewportSize, setViewportSize, glm::dvec2(100, 100))
    LATTICE_AUTO_PROPERTY(double, fieldOfView, setFieldOfView, glm::radians(60.0))
    LATTICE_AUTO_PROPERTY(double, zNear, setZNear, 0.1)
    LATTICE_AUTO_PROPERTY(double, zFar, setZFar, 100.0)
    LATTICE_AUTO_PROPERTY(double, orthoScale, setOrthoScale, 100.0)

    void emitCameraChanged();
};

} // namespace Editor
} // namespace Lattice
