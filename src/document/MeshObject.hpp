#pragma once
#include "Object.hpp"
#include <glm/glm.hpp>
#include "Material.hpp"

namespace Lattice {

namespace Mesh {
class Mesh;
}

namespace Document {

struct SubdivSettings {
    bool isEnabled = false;

    bool operator==(const SubdivSettings& other) const {
        return isEnabled == other.isEnabled;
    }
    bool operator!=(const SubdivSettings& other) const {
        return !operator==(other);
    }
};

class MeshObject final : public Object {
    Q_OBJECT
public:
    MeshObject();
    ~MeshObject() override;

    void setMesh(Mesh::Mesh mesh);
    auto& mesh() const { return *_mesh; }

    void setMaterials(std::vector<Material> materials);
    auto& materials() const { return _materials; }

    void setSubdivSettings(SubdivSettings settings);
    auto& subdivSettings() const { return _subdivSettings; }

    SP<Object> clone() const override;
    void toJSON(nlohmann::json& json) const override;
    void fromJSON(const nlohmann::json& json) override;

signals:
    void meshChanged(const Mesh::Mesh& mesh);
    void materialsChanged(const std::vector<Material>& materials);
    void subdivSettingsChanged(const SubdivSettings& settings);

private:
    void setMeshInternal(Mesh::Mesh mesh);
    void setMaterialsInternal(std::vector<Material> materials);
    void setSubdivSettingsInternal(SubdivSettings settings);

    std::unique_ptr<Mesh::Mesh> _mesh;
    std::vector<Material> _materials;
    SubdivSettings _subdivSettings;
};

}
}
