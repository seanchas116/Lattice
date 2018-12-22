#include "ThickLineShader.hpp"
#include "../../resource/Resource.hpp"

namespace Lattice {

ThickLineShader::ThickLineShader() :
    Shader(readResource("src/viewport/shaders/ThickLineShader.vert"), readResource("src/viewport/shaders/ThickLineShader.geom"), readResource("src/viewport/shaders/ThickLineShader.frag"))
{
    setUniform("zOffset", 0.f);
    setWidth(1.f);
    setColor(glm::vec3(0, 0, 0));
}

void ThickLineShader::setWidth(float width) {
    setUniform("width", width);
}

void ThickLineShader::setColor(glm::vec3 color) {
    setUniform("color", color);
}

void ThickLineShader::setMVPMatrix(glm::mat4 MVP) {
    setUniform("MVP", MVP);
}

void ThickLineShader::setViewportSize(glm::vec2 size) {
    setUniform("viewportSize", size);
}

} // namespace Lattice
