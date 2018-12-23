#pragma once
#include "Item.hpp"
#include <glm/glm.hpp>

namespace Lattice {

class OldMesh;

class MeshItem final : public Item {
    Q_OBJECT
public:
    MeshItem();

    const SP<OldMesh>& shape() const { return _shape; }
    void setShape(const SP<OldMesh> &shape);

    SP<Item> clone() const override;
    void toJSON(nlohmann::json& json) const override;
    void fromJSON(const nlohmann::json& json) override;

signals:
    void shapeChanged(const SP<OldMesh> &shape);

private:
    SP<OldMesh> _shape;
};

} // namespace Lattice
