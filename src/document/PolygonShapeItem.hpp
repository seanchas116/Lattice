#pragma once
#include "Item.hpp"
#include <glm/glm.hpp>

namespace Lattice {

class PolygonShape;

class PolygonShapeItem final : public Item {
    Q_OBJECT
public:
    PolygonShapeItem();

    const SP<PolygonShape>& shape() const { return _shape; }
    void setShape(const SP<PolygonShape> &shape);

    SP<Item> clone() const override;
    void toJSON(nlohmann::json& json) const override;
    void fromJSON(const nlohmann::json& json) override;

signals:
    void shapeChanged(const SP<PolygonShape> &shape);

private:
    SP<PolygonShape> _shape;
};

} // namespace Shapecraft
