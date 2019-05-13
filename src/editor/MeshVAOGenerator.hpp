#pragma once
#include "../support/Shorthands.hpp"
#include "../gl/VertexBuffer.hpp"
#include "../draw/Vertex.hpp"
#include <unordered_map>

namespace Lattice {

namespace Mesh {
class Mesh;
}
namespace GL {
class VAO;
}

namespace Editor {

class MeshVAOGenerator {
public:
    MeshVAOGenerator(const Mesh::Mesh& mesh);

    SP<GL::VAO> generateVertexVAO() const;
    SP<GL::VAO> generateEdgeVAO() const;
    std::unordered_map<uint32_t, SP<GL::VAO>> generateFaceVAOs() const;
    std::unordered_map<uint32_t, SP<GL::VAO>> generateSubdivFaceVAOs(int segmentCount) const;

private:
    const Mesh::Mesh& _mesh;
    SP<GL::VertexBuffer<Draw::PointLineVertex>> _vertexEdgeVertexBuffer;
};

} // namespace Viewport
} // namespace Lattice
