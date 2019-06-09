#pragma once
#include <QObject>
#include "../support/Camera.hpp"
#include "../support/Property.hpp"

namespace Lattice {
namespace Editor {

class CameraState : public QObject {
    Q_OBJECT

    LATTICE_AUTO_PROPERTY(Camera::Projection, projection, setProjection, Camera::Projection::Perspective)
    LATTICE_AUTO_PROPERTY(glm::dvec3, position, setPosition, glm::dvec3(0))
    LATTICE_AUTO_PROPERTY(glm::dvec3, eulerAngles, setEulerAngles, glm::dvec3(0))
    LATTICE_AUTO_PROPERTY(glm::dvec2, viewportSize, setViewportSize, glm::dvec2(100, 100))
    LATTICE_AUTO_PROPERTY(double, fieldOfView, setFieldOfView, glm::radians(60.0))
    LATTICE_AUTO_PROPERTY(double, zNear, setZNear, 0.1)
    LATTICE_AUTO_PROPERTY(double, zFar, setZFar, 100.0)
    LATTICE_AUTO_PROPERTY(double, orthoScale, setOrthoScale, 100.0)

public:
    CameraState();
};

} // namespace Editor
} // namespace Lattice
