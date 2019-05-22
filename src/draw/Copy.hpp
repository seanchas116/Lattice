#pragma once
#include "../gl/Shader.hpp"

namespace Lattice {

namespace GL {
class Texture;
class VAO;
}

namespace Draw {

class Copy : public QOpenGLExtraFunctions {
public:
    Copy();

    void copy(const SP<GL::Texture>& texture, const SP<GL::Texture>& depthTexture, float opacity);

private:
    GL::Shader _shader;
    SP<GL::VAO> _vao;
};

} // namespace Draw
} // namespace Lattice
