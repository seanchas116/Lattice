#pragma once
#include "Material.hpp"
#include "Object.hpp"
#include <glm/glm.hpp>

namespace Lattice {

namespace Mesh {
class Mesh;
}

namespace Document {

struct SubdivSettings {
    bool isEnabled = false;
    int segmentCount = 4;

    bool operator==(const SubdivSettings &other) const {
        return isEnabled == other.isEnabled && segmentCount == other.segmentCount;
    }
    bool operator!=(const SubdivSettings &other) const {
        return !operator==(other);
    }
};

struct MirrorSettings {
    glm::bvec3 isEnabled{false};

    bool operator==(const MirrorSettings &other) const {
        return isEnabled == other.isEnabled;
    }
    bool operator!=(const MirrorSettings &other) const {
        return !operator==(other);
    }
};

class MeshObject final : public Object {
    Q_OBJECT
  public:
    MeshObject();
    ~MeshObject() override;

    void setMesh(Mesh::Mesh mesh);
    auto &mesh() const { return *_mesh; }

    void setMaterials(std::vector<Material> materials);
    auto &materials() const { return _materials; }

    void setSubdivSettings(SubdivSettings settings);
    auto &subdivSettings() const { return _subdivSettings; }

    void setMirrorSettings(MirrorSettings settings);
    auto &mirrorSettings() const { return _mirrorSettings; }

    SP<Object> clone() const override;
    void toJSON(nlohmann::json &json) const override;
    void fromJSON(const nlohmann::json &json) override;

  signals:
    void meshChanged(const Mesh::Mesh &mesh);
    void materialsChanged(const std::vector<Material> &materials);
    void subdivSettingsChanged(const SubdivSettings &settings);
    void mirrorSettingsChanged(const MirrorSettings &settings);

  private:
    void setMeshInternal(Mesh::Mesh mesh);
    void setMaterialsInternal(std::vector<Material> materials);
    void setSubdivSettingsInternal(SubdivSettings settings);
    void setMirrorSettingsInternal(MirrorSettings settings);

    std::unique_ptr<Mesh::Mesh> _mesh;
    std::vector<Material> _materials;
    SubdivSettings _subdivSettings;
    MirrorSettings _mirrorSettings;
};

} // namespace Document
} // namespace Lattice
