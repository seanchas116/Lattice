#pragma once
#include <QObject>
#include "../support/OldCamera.hpp"
#include "../support/Property.hpp"

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

private:
    static glm::dvec3 orientationAngle(Orientation orientation);

    LATTICE_AUTO_PROPERTY(Orientation, orientation, setOrientation, Orientation::None)
    LATTICE_AUTO_PROPERTY(OldCamera::Projection, projection, setProjection, OldCamera::Projection::Perspective)
    LATTICE_AUTO_PROPERTY(glm::dvec3, position, setPosition, glm::dvec3(0))
    LATTICE_AUTO_PROPERTY(glm::dvec3, eulerAngles, setEulerAngles, glm::dvec3(0))
    LATTICE_AUTO_PROPERTY(glm::dvec2, viewportSize, setViewportSize, glm::dvec2(100, 100))
    LATTICE_AUTO_PROPERTY(double, fieldOfView, setFieldOfView, glm::radians(60.0))
    LATTICE_AUTO_PROPERTY(double, zNear, setZNear, 0.1)
    LATTICE_AUTO_PROPERTY(double, zFar, setZFar, 100.0)
    LATTICE_AUTO_PROPERTY(double, orthoScale, setOrthoScale, 100.0)
};

} // namespace Editor
} // namespace Lattice
