#pragma once
#include "../../gl/Shader.hpp"
#include "../../document/Mesh.hpp"
#include <unordered_map>
#include <QOpenGLExtraFunctions>

namespace Lattice {

class VAO;
class Camera;
class Texture;

class DrawMaterial final : protected QOpenGLExtraFunctions {
public:
    DrawMaterial();

    void draw(const SP<VAO>& vao, const glm::dmat4& matrix, const Camera& camera, const SP<MeshMaterial>& material);

    SP<Texture> getTexture(const QImage& image);

private:
    Shader _shader;
    std::unordered_map<qint64, SP<Texture>> _textures;
};

} // namespace Lattice
