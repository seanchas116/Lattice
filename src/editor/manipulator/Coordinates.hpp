#pragma once
#include "../../support/Camera.hpp"
#include "Constants.hpp"
#include <glm/gtx/transform.hpp>

namespace Lattice {
namespace Editor {
namespace Manipulator {

class Coordinates final {
public:
    Coordinates(const SP<Camera>& camera, glm::dvec3 targetPos) : targetPos(targetPos) {
        using namespace glm;

        auto [viewportPos, isInViewport] = camera->mapWorldToViewport(targetPos);
        this->isInViewport = isInViewport;
        if (!isInViewport) {
            return;
        }

        // TODO: calculate scale from desired arrow length in viewport space
        if (camera->projection() == Camera::Projection::Perspective) {
            dvec3 viewportPosFixedDepth(viewportPos.xy(), Constants::fixedDepth);
            dvec3 positionFixedDepth_worldSpace = camera->mapViewportToWorld(viewportPosFixedDepth);

            scale = 1.0 / double(camera->viewportSize().y) * 20.0;

            manipulatorToWorld = glm::scale(glm::translate(glm::dmat4(1), positionFixedDepth_worldSpace), dvec3(scale));
        } else {
            double scale = 1.0 / camera->orthoScale() * 50;
            manipulatorToWorld = glm::scale(glm::translate(targetPos), dvec3(scale));
        }
        dmat4 worldToCamera = camera->worldToCameraMatrix();
        manipulatorToCamera = worldToCamera * manipulatorToWorld;

        dmat4 targetToCamera = worldToCamera * glm::translate(targetPos);

        for (int axis = 0; axis < 3; ++axis) {
            arrowRaysInManipulatorSpace[axis] = {manipulatorToCamera[3].xyz(), manipulatorToCamera[axis].xyz()};
            axisRaysInCameraSpace[axis] = {targetToCamera[3].xyz(), targetToCamera[axis].xyz()};
        }
    }

    glm::dvec3 targetPos;
    bool isInViewport;
    glm::dmat4 manipulatorToWorld;
    glm::dmat4 manipulatorToCamera;
    double scale;
    std::array<Ray<double>, 3> arrowRaysInManipulatorSpace;
    std::array<Ray<double>, 3> axisRaysInCameraSpace;
};

}
} // namespace Editor
} // namespace Lattice
