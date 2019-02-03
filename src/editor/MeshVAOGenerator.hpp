#pragma once
#include "../support/Pointer.hpp"
#include <unordered_map>

namespace Lattice {

namespace Document {
class Mesh;
class MeshUVPoint;
class MeshMaterial;
}
namespace OldGL {
class VertexBuffer;
class LineVAO;
class PointVAO;
class VAO;
}

namespace Editor {

class MeshVAOGenerator {
public:
    MeshVAOGenerator(const SP<Document::Mesh>& mesh);

    SP<OldGL::PointVAO> generateVertexVAO() const;
    SP<OldGL::LineVAO> generateEdgeVAO() const;
    std::unordered_map<SP<Document::MeshMaterial>, SP<OldGL::VAO>> generateFaceVAOs() const;

private:
    SP<Document::Mesh> _mesh;
    SP<OldGL::VertexBuffer> _vertexBuffer;
    std::unordered_map<Document::MeshUVPoint*, uint32_t> _indices;
};

} // namespace Viewport
} // namespace Lattice
