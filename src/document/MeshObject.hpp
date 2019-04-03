#pragma once
#include "Object.hpp"
#include <glm/glm.hpp>

namespace Lattice {

namespace Mesh {
class Mesh;
}

namespace Document {

class MeshObject final : public Object {
    Q_OBJECT
public:
    MeshObject();

    const SP<Mesh::Mesh>& mesh() const { return _mesh; }

    SP<Object> clone() const override;
    void toJSON(nlohmann::json& json) const override;
    void fromJSON(const nlohmann::json& json) override;

signals:
    void meshChangedInLastTick();

private:
    void handleMeshChange();

    SP<Mesh::Mesh> _mesh;
    bool _meshChangedInTick = false;
};

}
}
