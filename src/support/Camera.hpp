#pragma once
#include <optional>
#include <glm/glm.hpp>
#include "Location.hpp"
#include "Ray.hpp"
#include "Pointer.hpp"
#include <QtGlobal>

namespace Lattice {

class Camera final {
    Q_DISABLE_COPY(Camera)
public:
    enum class Projection {
        Perspective,
        Orthographic,
    };

    Camera();

    Projection projection() const { return _projection; }
    void setProjection(Projection projection);

    Location location() const { return _location; }
    void setLocation(const Location &location);

    void lookFront();

    glm::dvec2 viewSize() const { return _viewSize; }
    void setViewSize(const glm::dvec2 &viewSize);

    double fieldOfView() const { return _fieldOfView; }
    void setFieldOfView(double fieldOfView);

    double zNear() const { return _zNear; }
    void setZNear(double zNear);

    double zFar() const { return _zFar; }
    void setZFar(double zFar);

    double orthoScale() const { return _orthoScale; }
    void setOrthoScale(double orthoScale);

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

private:
    void updateMatrix();

    Projection _projection = Projection::Perspective;

    Location _location;

    glm::dvec2 _viewSize;
    double _fieldOfView;
    double _zNear;
    double _zFar;

    double _orthoScale = 100;

    glm::dmat4 _cameraToScreenMatrix;
    glm::dmat4 _worldToCameraMatrix;
    glm::dmat4 _worldToScreenMatrix;
};

} // namespace Lattice
