#pragma once
#include "Item.hpp"
#include <glm/glm.hpp>

namespace Lattice::Document {

class Mesh;

class MeshItem final : public Item {
    Q_OBJECT
public:
    MeshItem();

    const SP<Mesh>& mesh() const { return _mesh; }

    SP<Item> clone() const override;
    void toJSON(nlohmann::json& json) const override;
    void fromJSON(const nlohmann::json& json) override;

signals:

private:
    SP<Mesh> _mesh;
};

} // namespace Lattice
