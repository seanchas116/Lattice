#pragma once
#include "Object.hpp"
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

    auto& oldMesh() const { return _oldMesh; }

    SP<Object> clone() const override;
    void toJSON(nlohmann::json& json) const override;
    void fromJSON(const nlohmann::json& json) override;

signals:
    void oldMeshChangedInLastTick();

private:
    void handleOldMeshChange();

    SP<OldMesh::Mesh> _oldMesh;
    bool _oldMeshChangedInTick = false;
};

}
}
