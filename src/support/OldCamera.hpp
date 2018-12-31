#pragma once

#include <glm/glm.hpp>

namespace Lattice {

class OldCamera final {
public:
    OldCamera();

    glm::vec3 pos() const { return _pos; }
    void setPos(glm::vec3 pos);

    float yaw() const { return _yaw; }
    void setYaw(float yaw);

    float pitch() const { return _pitch; }
    void setPitch(float pitch);

    glm::vec3 direction() const { return _direction; }
    glm::vec3 right() const { return _right; }
    glm::vec3 up() const { return _up; }
    glm::mat4 matrix() const { return _matrix; }

private:
    void update();

    glm::vec3 _pos = {0, 10, 10};
    float _yaw = float(-M_PI);
    float _pitch = 0;
    glm::vec3 _direction;
    glm::vec3 _right;
    glm::vec3 _up;

    glm::mat4 _matrix;
};

} // namespace Lattice
