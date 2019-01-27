#pragma once
#include <optional>
#include <glm/glm.hpp>
#include "Location.hpp"
#include "Ray.hpp"
#include "Pointer.hpp"
#include <QObject>
#include "Property.hpp"

namespace Lattice {

class Camera final : public QObject {
    Q_OBJECT

public:
    enum class Projection {
        Perspective,
        Orthographic,
    };

    Camera();

    void lookFront();

    glm::dmat4 worldToCameraMatrix() const { return _worldToCameraMatrix; }
    glm::dmat4 cameraToScreenMatrix() const { return _cameraToScreenMatrix; }
    glm::dmat4 worldToScreenMatrix() const { return _worldToScreenMatrix; }

    std::pair<glm::dvec3, bool> mapWorldToScreen(glm::dvec3 worldPos) const;
    glm::dvec3 mapScreenToWorld(glm::dvec3 screenPosWithDepth) const;

    // screen coordinate range:
    //  0 <= x <= logical window width
    //  0 <= y <= logical window height
    //  0 <= z <= 1 (1 is farthest from camera)
    std::pair<glm::dvec3, bool> mapCameraToScreen(glm::dvec3 cameraPos) const;
    glm::dvec3 mapScreenToCamera(glm::dvec3 screenPosWithDepth) const;

    Ray<double> cameraMouseRay(glm::dvec2 screenPos) const;
    Ray<double> worldMouseRay(glm::dvec2 screenPos) const;

signals:
    void changed();

private:
    LATTICE_AUTO_PROPERTY(Projection, projection, setProjection, Projection::Perspective)
    LATTICE_AUTO_PROPERTY(Location, location, setLocation, {})
    LATTICE_AUTO_PROPERTY(glm::dvec2, viewSize, setViewSize, glm::dvec2(100, 100))
    LATTICE_AUTO_PROPERTY(double, fieldOfView, setFieldOfView, glm::radians(60.0))
    LATTICE_AUTO_PROPERTY(double, zNear, setZNear, 0.1)
    LATTICE_AUTO_PROPERTY(double, zFar, setZFar, 100.0)
    LATTICE_AUTO_PROPERTY(double, orthoScale, setOrthoScale, 100.0)

    void updateMatrix();

    glm::dmat4 _cameraToScreenMatrix;
    glm::dmat4 _worldToCameraMatrix;
    glm::dmat4 _worldToScreenMatrix;
};

} // namespace Lattice
