#include "MeshRenderer.hpp"
#include "../gl/VAO.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/VertexBuffer.hpp"

namespace Lattice {

MeshRenderer::MeshRenderer() {
    auto vbo = std::make_shared<VertexBuffer>();
    _faceVAO = std::make_shared<VAO>(vbo);
    _edgeVAO = std::make_shared<LineVAO>(vbo);
}

}
