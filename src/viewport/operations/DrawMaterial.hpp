#pragma once
#include "../../gl/Shader.hpp"
#include "../../document/Mesh.hpp"
#include <unordered_map>
#include <QOpenGLExtraFunctions>

namespace Lattice {

class VAO;
class Projection;
class Texture;

class DrawMaterial : protected QOpenGLExtraFunctions {
public:
    DrawMaterial();

    void draw(const SP<VAO>& vao, const glm::mat4& matrix, const Projection& projection, const SP<MeshMaterial>& material);

    SP<Texture> getTexture(const QImage& image);

private:
    Shader _shader;
    std::unordered_map<qint64, SP<Texture>> _textures;
};

} // namespace Lattice
