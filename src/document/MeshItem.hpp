#pragma once
#include "Item.hpp"
#include <glm/glm.hpp>

namespace Lattice {

namespace Mesh {
class Mesh;
}

namespace Document {

class MeshItem final : public Item {
    Q_OBJECT
public:
    MeshItem();

    const SP<Mesh::Mesh>& mesh() const { return _mesh; }

    SP<Item> clone() const override;
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
