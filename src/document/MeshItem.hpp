#pragma once
#include "Item.hpp"
#include <glm/glm.hpp>

namespace Lattice {

class Mesh;

class MeshItem final : public Item {
    Q_OBJECT
public:
    MeshItem();

    const SP<Mesh>& shape() const { return _shape; }
    void setShape(const SP<Mesh> &shape);

    SP<Item> clone() const override;
    void toJSON(nlohmann::json& json) const override;
    void fromJSON(const nlohmann::json& json) override;

signals:
    void shapeChanged(const SP<Mesh> &shape);

private:
    SP<Mesh> _shape;
};

} // namespace Lattice
