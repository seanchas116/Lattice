#pragma once
#include "../draw/Vertex.hpp"
#include "../gl/VertexBuffer.hpp"
#include "../support/Shorthands.hpp"
#include <meshlib/Handle.hpp>
#include <unordered_map>

namespace meshlib {
class Mesh;
}

namespace Lattice {

namespace GL {
class VAO;
}

namespace Editor {

class MeshVAOGenerator {
  public:
    MeshVAOGenerator(const meshlib::Mesh &mesh);

    SP<GL::VAO> generateVertexVAO() const;
    SP<GL::VAO> generateEdgeVAO() const;
    std::unordered_map<meshlib::MaterialHandle, SP<GL::VAO>> generateFaceVAOs() const;
    std::unordered_map<meshlib::MaterialHandle, SP<GL::VAO>> generateSubdivFaceVAOs(int segmentCount) const;

  private:
    const meshlib::Mesh &_mesh;
    SP<GL::VertexBuffer<Draw::PointLineVertex>> _vertexEdgeVertexBuffer;
};

} // namespace Editor
} // namespace Lattice
