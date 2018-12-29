#include "DrawMaterial.hpp"
#include "../../gl/VAO.hpp"
#include "../../gl/Texture.hpp"
#include "../../resource/Resource.hpp"
#include "../../support/Projection.hpp"

namespace Lattice {

DrawMaterial::DrawMaterial() :
    _shader(readResource("src/viewport/operations/DrawMaterial.vert"), std::string(), readResource("src/viewport/operations/DrawMaterial.frag"))
{
}

void DrawMaterial::draw(const SP<VAO> &vao, const glm::mat4 &matrix, const Projection &projection, const SP<MeshMaterial> &material) {
    _shader.bind();
    _shader.setUniform("diffuse", material->baseColor());
    _shader.setUniform("ambient", glm::vec3(0));
    _shader.setUniform("MV", matrix);
    _shader.setUniform("MVP", projection.matrix() * matrix);
    vao->draw();
}

SP<Texture> DrawMaterial::getTexture(const QImage &image) {
    if (auto it = _textures.find(image.cacheKey()); it != _textures.end()) {
        return it->second;
    }

    auto rgbaTexture = image.convertToFormat(QImage::Format_RGBA8888_Premultiplied);
    glm::ivec2 size(rgbaTexture.width(), rgbaTexture.height());

    auto texture = std::make_shared<Texture>(size, rgbaTexture.bits());
    _textures[image.cacheKey()] = texture;
    return texture;
}

} // namespace Lattice
