#pragma once
#include "../../gl/Shader.hpp"
#include "../../document/Mesh.hpp"

namespace Lattice {

class VAO;
class Projection;

class DrawMaterial {
public:
    DrawMaterial();

    void draw(const SP<VAO>& vao, const glm::mat4& matrix, const Projection& projection, const SP<MeshMaterial>& material);

private:
    Shader _shader;
};

} // namespace Lattice
