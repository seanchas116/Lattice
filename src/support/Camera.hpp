#pragma once
#include "Ray.hpp"
#include <glm/glm.hpp>
#include <tuple>

namespace Lattice {

class Camera {
  public:
    enum class Projection {
        Perspective,
        Orthographic,
    };

    auto projection() const { return _projection; }
    auto cameraToWorldMatrix() const { return _cameraToWorldMatrix; }
    auto viewportSize() const { return _viewportSize; }
    auto fieldOfView() const { return _fieldOfView; }
    auto zNear() const { return _zNear; }
    auto zFar() const { return _zFar; }
    auto orthoScale() const { return _orthoScale; }

    auto cameraToViewportMatrix() const { return _cameraToViewportMatrix; }
    auto worldToCameraMatrix() const { return _worldToCameraMatrix; }
    auto worldToViewportMatrix() const { return _worldToViewportMatrix; }

    static Camera perspective(glm::dmat4 cameraToWorldMatrix, glm::dvec2 viewportSize, double fieldOfView = glm::radians(60.0), double zNear = 0.1, double zFar = 100.0);
    static Camera orthographic(glm::dmat4 cameraToWorldMatrix, glm::dvec2 viewportSize, double scale);

    std::pair<glm::dvec3, bool> mapModelToViewport(const glm::dmat4 &modelMatrix, glm::dvec3 worldPos) const;
    glm::dvec3 mapViewportToModel(const glm::dmat4 &modelMatrix, glm::dvec3 viewportPosWithDepth) const;

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
    Ray<double> modelMouseRay(const glm::dmat4 &modelMatrix, glm::dvec2 viewportPos) const;

  private:
    Camera() {}

    Projection _projection;
    glm::dmat4 _cameraToWorldMatrix;
    glm::dvec2 _viewportSize;
    double _fieldOfView;
    double _zNear;
    double _zFar;
    double _orthoScale;

    glm::dmat4 _cameraToViewportMatrix;
    glm::dmat4 _worldToCameraMatrix;
    glm::dmat4 _worldToViewportMatrix;
};

} // namespace Lattice
