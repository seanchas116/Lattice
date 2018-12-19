#include "SolidShader.hpp"
#include "../resource/Resource.hpp"

namespace Lattice {

SolidShader::SolidShader() :
    Shader(readResource("src/viewport/Solid.vert"), std::string(), readResource("src/viewport/Solid.frag"))
{

}

void SolidShader::setDiffuse(glm::vec3 diffuse) {
    setUniform("diffuse", diffuse);
}

void SolidShader::setMVPMatrix(glm::mat4 MV, glm::mat4 MVP) {
    setUniform("MV", MV);
    setUniform("MVP", MVP);
}

} // namespace Lattice
