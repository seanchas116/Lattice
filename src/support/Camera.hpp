#pragma once
#include <optional>
#include <glm/glm.hpp>
#include "Location.hpp"
#include "Ray.hpp"
#include "Shorthands.hpp"
#include <QObject>
#include "Property.hpp"

namespace Lattice {

struct OrientationAngles {
    inline static const glm::dvec3 front {0, 0, 0};
    inline static const glm::dvec3 back {0, M_PI, 0};
    inline static const glm::dvec3 right {0, M_PI * 0.5, 0};
    inline static const glm::dvec3 left {0, M_PI * 1.5, 0};
    inline static const glm::dvec3 top {M_PI * -0.5, 0, 0};
    inline static const glm::dvec3 bottom {M_PI * 0.5, 0, 0};
};

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
    LATTICE_AUTO_PROPERTY(glm::dvec2, viewSize, setViewSize, glm::dvec2(100, 100))
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
