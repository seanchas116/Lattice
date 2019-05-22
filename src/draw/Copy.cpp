#include "Copy.hpp"
#include "../gl/Texture.hpp"
#include "../gl/VAO.hpp"
#include "../resource/Resource.hpp"

namespace Lattice {
namespace Draw {

namespace {

struct CopyVAOAttribute {
    glm::vec2 texCoord;
};

SP<GL::VAO> createCopyVAO() {
    std::vector<CopyVAOAttribute> attributes = {
        {glm::vec2(0, 0)},
        {glm::vec2(1, 0)},
        {glm::vec2(1, 1)},
        {glm::vec2(0, 1)},
    };

    auto vbo = makeShared<GL::VertexBuffer<CopyVAOAttribute>>(attributes);
    return makeShared<GL::VAO>(vbo, GL::Primitive::TriangleFan);
}

}

Copy::Copy() : _shader(Resource::read("src/draw/Copy.vert"), {}, Resource::read("src/draw/Copy.frag")),
               _vao(createCopyVAO())
{
    initializeOpenGLFunctions();
}

void Copy::copy(const SP<GL::Texture> &texture, const SP<GL::Texture> &depthTexture, float opacity) {
    _shader.bind();

    glActiveTexture(GL_TEXTURE0);
    texture->bind();
    glActiveTexture(GL_TEXTURE1);
    depthTexture->bind();

    _shader.setUniform("colorSampler", 0);
    _shader.setUniform("depthSampler", 0);
    _shader.setUniform("opacity", opacity);
    _vao->draw();
}

} // namespace Draw
} // namespace Lattice
