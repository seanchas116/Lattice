#include "MeshItem.hpp"
#include "OldMesh.hpp"
#include "../support/Debug.hpp"
#include <QPainterPath>
#include <nlohmann/json.hpp>

using namespace glm;

namespace Lattice {

MeshItem::MeshItem() : _shape(std::make_shared<OldMesh>()) {
    connect(this, &MeshItem::shapeChanged, this, &Item::changed);

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

SP<Item> MeshItem::clone() const {
    auto cloned = std::make_shared<MeshItem>();
    cloned->_shape = _shape->clone();
    return cloned;
}

void MeshItem::toJSON(nlohmann::json &json) const {
    Item::toJSON(json);
    // TODO
    // _shape->toJSON(json["shape"]);
}

void MeshItem::fromJSON(const nlohmann::json &json) {
    Item::fromJSON(json);
    // TODO
    //_shape->fromJSON(json["shape"]);
}

void MeshItem::setShape(const SP<OldMesh> &shape) {
    emit shapeChanged(shape);
    _shape = shape;
}

} // namespace Lattice
