#pragma once
#include "../support/Shorthands.hpp"
#include "../gl/VertexBuffer.hpp"
#include <unordered_map>

namespace Lattice {

namespace Document {
class Mesh;
class MeshUVPoint;
class MeshMaterial;
}
namespace GL {
class VAO;
}

namespace Editor {

class MeshVAOGenerator {
public:
    MeshVAOGenerator(const SP<Document::Mesh>& mesh);

    SP<GL::VAO> generateVertexVAO() const;
    SP<GL::VAO> generateEdgeVAO() const;
    std::unordered_map<SP<Document::MeshMaterial>, SP<GL::VAO>> generateFaceVAOs() const;

private:
    SP<Document::Mesh> _mesh;
    SP<GL::VertexBuffer<GL::Vertex>> _vertexBuffer;
    std::unordered_map<Document::MeshUVPoint*, uint32_t> _indices;
};

} // namespace Viewport
} // namespace Lattice
