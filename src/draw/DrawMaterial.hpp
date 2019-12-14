#pragma once
#include "../gl/Shader.hpp"
#include "Material.hpp"
#include <QOpenGLExtraFunctions>
#include <unordered_map>

namespace Lattice {
class Camera;

namespace GL {
class VAO;
class Texture;
} // namespace GL

namespace Draw {

class DrawMaterial final : protected QOpenGLExtraFunctions {
  public:
    DrawMaterial();

    void draw(const SP<GL::VAO> &vao, const glm::dmat4 &matrix, const Camera &camera, const Material &material);

    SP<GL::Texture> getTexture(const QImage &image);

  private:
    GL::Shader _shader;
    std::unordered_map<qint64, SP<GL::Texture>> _textures;
};

} // namespace Draw
} // namespace Lattice
