#pragma once
#include "../support/Shorthands.hpp"
#include "../gl/VertexBuffer.hpp"
#include "../draw/Vertex.hpp"
#include <unordered_map>

namespace Lattice {

namespace OldMesh {
class Mesh;
class UVPoint;
class Material;
}
namespace GL {
class VAO;
}

namespace Editor {

class MeshVAOGenerator {
public:
    MeshVAOGenerator(const SP<OldMesh::Mesh>& mesh);

    SP<GL::VAO> generateVertexVAO() const;
    SP<GL::VAO> generateEdgeVAO() const;
    std::unordered_map<SP<OldMesh::Material>, SP<GL::VAO>> generateFaceVAOs() const;

private:
    SP<OldMesh::Mesh> _mesh;
    SP<GL::VertexBuffer<Draw::PointLineVertex>> _vertexEdgeVertexBuffer;
    std::unordered_map<OldMesh::UVPoint*, uint32_t> _indices;
};

} // namespace Viewport
} // namespace Lattice
