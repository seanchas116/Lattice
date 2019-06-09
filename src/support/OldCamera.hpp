#pragma once
#include <optional>
#include <glm/glm.hpp>
#include "Location.hpp"
#include "Ray.hpp"
#include "Shorthands.hpp"
#include <QObject>
#include "Property.hpp"

namespace Lattice {

class OldCamera final : public QObject {
    Q_OBJECT

public:
    enum class Projection {
        Perspective,
        Orthographic,
    };

    enum class Orientation {
        Front,
        Back,
        Right,
        Left,
        Top,
        Bottom,
    };
    static glm::dvec3 orientationAngle(Orientation orientation);

    OldCamera();

    void lookOrientation(Orientation orientation);
    bool isLookingOrientation(Orientation orientation) const;

    glm::dmat4 worldToCameraMatrix() const { return _worldToCameraMatrix; }
    glm::dmat4 cameraToViewportMatrix() const { return _cameraToViewportMatrix; }
    glm::dmat4 worldToViewportMatrix() const { return _worldToViewportMatrix; }

    std::pair<glm::dvec3, bool> mapModelToViewport(const glm::dmat4& modelMatrix, glm::dvec3 worldPos) const;
    glm::dvec3 mapViewportToModel(const glm::dmat4& modelMatrix, glm::dvec3 viewportPosWithDepth) const;

    std::pair<glm::dvec3, bool> mapWorldToViewport(glm::dvec3 worldPos) const;
    glm::dvec3 mapViewportToWorld(glm::dvec3 viewportPosWithDepth) const;

    // viewport coordinate range:
    //  0 <= x <= logical window width
    //  0 <= y <= logical window height
    //  0 <= z <= 1 (1 is farthest from camera)
    std::pair<glm::dvec3, bool> mapCameraToViewport(glm::dvec3 cameraPos) const;
    glm::dvec3 mapViewportToCamera(glm::dvec3 viewportPosWithDepth) const;

    Ray<double> cameraMouseRay(glm::dvec2 viewportPos) const;
    Ray<double> worldMouseRay(glm::dvec2 viewportPos) const;
    Ray<double> modelMouseRay(const glm::dmat4& modelMatrix, glm::dvec2 viewportPos) const;

signals:
    void changed();

private:
    LATTICE_AUTO_PROPERTY(Projection, projection, setProjection, Projection::Perspective)
    LATTICE_AUTO_PROPERTY(Location, location, setLocation, {})
    LATTICE_AUTO_PROPERTY(glm::dvec2, viewportSize, setViewportSize, glm::dvec2(100, 100))
    LATTICE_AUTO_PROPERTY(double, fieldOfView, setFieldOfView, glm::radians(60.0))
    LATTICE_AUTO_PROPERTY(double, zNear, setZNear, 0.1)
    LATTICE_AUTO_PROPERTY(double, zFar, setZFar, 100.0)
    LATTICE_AUTO_PROPERTY(double, orthoScale, setOrthoScale, 100.0)

    void updateMatrix();

    glm::dmat4 _cameraToViewportMatrix;
    glm::dmat4 _worldToCameraMatrix;
    glm::dmat4 _worldToViewportMatrix;
};

} // namespace Lattice
