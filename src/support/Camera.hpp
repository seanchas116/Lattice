#pragma once
#include <optional>
#include <glm/glm.hpp>
#include "Location.hpp"
#include "Line.hpp"

namespace Lattice {

class Camera final {
public:
    Camera();

    Location location() const { return _location; }
    void setLocation(const Location &location);

    glm::dvec2 viewSize() const { return _viewSize; }
    void setViewSize(const glm::dvec2 &viewSize);

    double fieldOfView() const { return _fieldOfView; }
    void setFieldOfView(double fieldOfView);

    double zNear() const { return _zNear; }
    void setZNear(double zNear);

    double zFar() const { return _zFar; }
    void setZFar(double zFar);

    glm::dmat4 worldToCameraMatrix() const { return _worldToCameraMatrix; }
    glm::dmat4 cameraToScrenMatrix() const { return _cameraToScreenMatrix; }
    glm::dmat4 worldToScreenMatrix() const { return _worldToScreenMatrix; }

    std::pair<glm::dvec3, bool> mapWorldToScreen(glm::dvec3 worldPos) const;
    glm::dvec3 mapScreenToWorld(glm::dvec3 screenPosWithDepth) const;

    std::pair<glm::dvec3, bool> mapCameraToScreen(glm::dvec3 cameraPos) const;
    glm::dvec3 mapScreenToCamera(glm::dvec3 screenPosWithDepth) const;

    Line cameraMouseRay(glm::dvec2 screenPos) const;

private:
    void updateMatrix();

    Location _location;

    glm::dvec2 _viewSize;
    double _fieldOfView;
    double _zNear;
    double _zFar;
    glm::dmat4 _cameraToScreenMatrix;
    glm::dmat4 _worldToCameraMatrix;
    glm::dmat4 _worldToScreenMatrix;
};

} // namespace Lattice
