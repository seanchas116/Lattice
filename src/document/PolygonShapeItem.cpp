#include "PolygonShapeItem.hpp"
#include "PolygonShape.hpp"
#include "../support/Debug.hpp"
#include <QPainterPath>
#include <nlohmann/json.hpp>

using namespace glm;

namespace Lattice {

PolygonShapeItem::PolygonShapeItem() : _shape(std::make_shared<PolygonShape>()) {
    connect(this, &PolygonShapeItem::shapeChanged, this, &Item::changed);

    auto v1 = _shape->addVertex(vec3(0, 0, 0));
    auto v2 = _shape->addVertex(vec3(0, 0, 1));
    auto v3 = _shape->addVertex(vec3(1, 0, 1));
    auto v4 = _shape->addVertex(vec3(1, 0, 0));
    auto e1 = _shape->addEdge(v1, v2);
    auto e2 = _shape->addEdge(v2, v3);
    auto e3 = _shape->addEdge(v3, v4);
    auto e4 = _shape->addEdge(v4, v1);
    auto f = _shape->addFace({e1, e2, e3, e4});
    auto vertices = f->vertices();

    for (auto&& v : vertices) {
        qDebug() << v->position();
    }
    qDebug() << f->normal();
}

SP<Item> PolygonShapeItem::clone() const {
    auto cloned = std::make_shared<PolygonShapeItem>();
    cloned->_shape = _shape->clone();
    return cloned;
}

void PolygonShapeItem::toJSON(nlohmann::json &json) const {
    Item::toJSON(json);
    // TODO
    // _shape->toJSON(json["shape"]);
}

void PolygonShapeItem::fromJSON(const nlohmann::json &json) {
    Item::fromJSON(json);
    // TODO
    //_shape->fromJSON(json["shape"]);
}

void PolygonShapeItem::setShape(const SP<PolygonShape> &shape) {
    emit shapeChanged(shape);
    _shape = shape;
}

} // namespace Shapecraft
