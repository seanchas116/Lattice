#pragma once
#include "../support/Pointer.hpp"
#include "../gl/VertexBuffer.hpp"
#include <unordered_map>

namespace Lattice {

namespace Document {
class Mesh;
class MeshUVPoint;
class MeshMaterial;
}
namespace GL {
class OldVertexBuffer;
class LineVAO;
class PointVAO;
class VAO;
}

namespace Editor {

class MeshVAOGenerator {
public:
    MeshVAOGenerator(const SP<Document::Mesh>& mesh);

    SP<GL::PointVAO> generateVertexVAO() const;
    SP<GL::LineVAO> generateEdgeVAO() const;
    std::unordered_map<SP<Document::MeshMaterial>, SP<GL::VAO>> generateFaceVAOs() const;

private:
    SP<Document::Mesh> _mesh;
    SP<GL::VertexBuffer<GL::Vertex>> _vertexBuffer;
    std::unordered_map<Document::MeshUVPoint*, uint32_t> _indices;
};

} // namespace Viewport
} // namespace Lattice
