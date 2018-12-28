#include "MeshItem.hpp"
#include "Mesh.hpp"
#include "../support/Debug.hpp"
#include <QPainterPath>
#include <nlohmann/json.hpp>

using namespace glm;

namespace Lattice {

MeshItem::MeshItem() : _mesh(std::make_shared<Mesh>()) {
    connect(this, &MeshItem::meshChanged, this, &Item::changed);

    auto v1 = _mesh->addVertex(vec3(0, 0, 0));
    auto v2 = _mesh->addVertex(vec3(0, 0, 1));
    auto v3 = _mesh->addVertex(vec3(1, 0, 1));
    auto v4 = _mesh->addVertex(vec3(1, 0, 0));
    auto m = _mesh->addMaterial();
    auto f = _mesh->addFace({v1, v2, v3, v4}, m);
    auto vertices = f->vertices();

    for (auto&& v : vertices) {
        qDebug() << v->position();
    }
    qDebug() << f->normal();
}

SP<Item> MeshItem::clone() const {
    auto cloned = std::make_shared<MeshItem>();
    cloned->_mesh = _mesh->clone();
    return cloned;
}

void MeshItem::toJSON(nlohmann::json &json) const {
    Item::toJSON(json);
    throw std::runtime_error("TODO");
    // TODO
    // _shape->toJSON(json["shape"]);
}

void MeshItem::fromJSON(const nlohmann::json &json) {
    Item::fromJSON(json);
    throw std::runtime_error("TODO");
    // TODO
    //_shape->fromJSON(json["shape"]);
}

void MeshItem::setMesh(const SP<Mesh> &mesh) {
    emit meshChanged(mesh);
    _mesh = mesh;
}

} // namespace Lattice
