#pragma once
#include "Item.hpp"
#include <glm/glm.hpp>

namespace Lattice {

class MeshShape;

class MeshItem final : public Item {
    Q_OBJECT
public:
    MeshItem();

    const SP<MeshShape>& shape() const { return _shape; }
    void setShape(const SP<MeshShape> &shape);

    SP<Item> clone() const override;
    void toJSON(nlohmann::json& json) const override;
    void fromJSON(const nlohmann::json& json) override;

signals:
    void shapeChanged(const SP<MeshShape> &shape);

private:
    SP<MeshShape> _shape;
};

} // namespace Lattice
