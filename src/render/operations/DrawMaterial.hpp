#pragma once
#include "../../gl/Shader.hpp"
#include "../../document/Mesh.hpp"
#include <unordered_map>
#include <QOpenGLExtraFunctions>

namespace Lattice {
class Camera;
}

namespace Lattice::OldGL {
class VAO;
class Texture;
}

namespace Lattice::Render {

class DrawMaterial final : protected QOpenGLExtraFunctions {
public:
    DrawMaterial();

    void draw(const SP<OldGL::VAO>& vao, const glm::dmat4& matrix, const SP<Camera>& camera, const SP<Document::MeshMaterial>& material);

    SP<OldGL::Texture> getTexture(const QImage& image);

private:
    OldGL::Shader _shader;
    std::unordered_map<qint64, SP<OldGL::Texture>> _textures;
};

} // namespace Lattice
