#pragma once
#include "../gl/Shader.hpp"
#include "Material.hpp"
#include <unordered_map>
#include <QOpenGLExtraFunctions>

namespace Lattice {
class Camera;

namespace GL {
class VAO;
class Texture;
}

namespace Draw {

class DrawMaterial final : protected QOpenGLExtraFunctions {
public:
    DrawMaterial();

    void draw(const SP<GL::VAO>& vao, const glm::dmat4& matrix, const Camera& camera, const Material& material);

    SP<GL::Texture> getTexture(const QImage& image);

private:
    GL::Shader _shader;
    std::unordered_map<qint64, SP<GL::Texture>> _textures;
};

}
} // namespace Lattice
