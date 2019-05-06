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
    ~MeshObject() override;

    void setMesh(Mesh::Mesh mesh);
    auto& mesh() const { return *_mesh; }

    void setMaterials(std::vector<Material> materials);
    auto& materials() const { return _materials; }

    SP<Object> clone() const override;
    void toJSON(nlohmann::json& json) const override;
    void fromJSON(const nlohmann::json& json) override;

signals:
    void meshChanged(const Mesh::Mesh& mesh);
    void materialsChanged(const std::vector<Material>& materials);

private:
    void setMeshInternal(Mesh::Mesh mesh);

    std::unique_ptr<Mesh::Mesh> _mesh;
    std::vector<Material> _materials;
};

}
}
