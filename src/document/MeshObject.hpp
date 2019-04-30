#pragma once
#include "Object.hpp"
#include "../mesh/Mesh.hpp"
#include <glm/glm.hpp>

namespace Lattice {

namespace OldMesh {
class Mesh;
}

namespace Document {

class MeshObject final : public Object {
    Q_OBJECT
public:
    MeshObject();

    void setMesh(Mesh::Mesh&& mesh);
    auto& mesh() const { return _mesh; }

    auto& oldMesh() const { return _oldMesh; }

    SP<Object> clone() const override;
    void toJSON(nlohmann::json& json) const override;
    void fromJSON(const nlohmann::json& json) override;

signals:
    void meshChanged(const Mesh::Mesh& mesh);
    void oldMeshChangedInLastTick();

private:
    void handleOldMeshChange();

    Mesh::Mesh _mesh;
    SP<OldMesh::Mesh> _oldMesh;
    bool _oldMeshChangedInTick = false;
};

}
}
