#pragma once
#include "Object.hpp"
#include <glm/glm.hpp>
#include "Material.hpp"

namespace Lattice {

namespace Mesh {
class Mesh;
}
namespace OldMesh {
class Mesh;
}

namespace Document {

class MeshObject final : public Object {
    Q_OBJECT
public:
    MeshObject();

    void setMesh(const SP<Mesh::Mesh> mesh);
    auto& mesh() const { return _mesh; }

    void setMaterials(const std::vector<Material>& materials);
    auto& materials() const { return _materials; }

    auto& oldMesh() const { return _oldMesh; }

    SP<Object> clone() const override;
    void toJSON(nlohmann::json& json) const override;
    void fromJSON(const nlohmann::json& json) override;

signals:
    void meshChanged(const SP<Mesh::Mesh>& mesh);
    void materialsChanged(const std::vector<Material>& materials);
    void oldMeshChangedInLastTick();

private:
    void handleOldMeshChange();

    SP<Mesh::Mesh> _mesh;
    std::vector<Material> _materials;

    SP<OldMesh::Mesh> _oldMesh;
    bool _oldMeshChangedInTick = false;
};

}
}
