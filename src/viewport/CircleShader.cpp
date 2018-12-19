#include "CircleShader.hpp"
#include "../resource/Resource.hpp"

namespace Lattice {

CircleShader::CircleShader() :
    Shader(readResource("src/viewport/Circle.vert"), readResource("src/viewport/Circle.geom"), readResource("src/viewport/Circle.frag"))
{
    setWidth(1.f);
    setColor(glm::vec3(0, 0, 0));
}

void CircleShader::setWidth(float width) {
    setUniform("width", width);
}

void CircleShader::setColor(glm::vec3 color) {
    setUniform("color", color);
}

void CircleShader::setMVPMatrix(glm::mat4 MVP) {
    setUniform("MVP", MVP);
}

void CircleShader::setViewportSize(glm::vec2 size) {
    setUniform("viewportSize", size);
}

} // namespace Lattice
