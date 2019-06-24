#include "MeshObject.hpp"
#include "../mesh/Mesh.hpp"
#include "../support/Debug.hpp"
#include "../support/PropertyChange.hpp"
#include <nlohmann/json.hpp>
#include <QTimer>

using namespace glm;

namespace Lattice {
namespace Document {

MeshObject::MeshObject() :
    _mesh(std::make_unique<Mesh::Mesh>()),
    _materials({Material()}) {
}

MeshObject::~MeshObject() {
}

void MeshObject::setMesh(Mesh::Mesh mesh) {
    auto self = *dynamicPointerCast<MeshObject>(sharedFromThis());
    auto change = makeShared<PropertyChange<MeshObject, Mesh::Mesh>>(self, std::move(mesh), &MeshObject::mesh, &MeshObject::setMeshInternal);
    addChange(change);
}

void MeshObject::setMaterials(std::vector<Material> materials) {
    auto self = *dynamicPointerCast<MeshObject>(sharedFromThis());
    auto change = makeShared<PropertyChange<MeshObject, std::vector<Material>>>(self, std::move(materials), &MeshObject::materials, &MeshObject::setMaterialsInternal);
    addChange(change);
}

void MeshObject::setMeshInternal(Mesh::Mesh mesh) {
    *_mesh = std::move(mesh);
    emit meshChanged(*_mesh);
}

void MeshObject::setMaterialsInternal(std::vector<Material> materials) {
    if (_materials == materials) {
        return;
    }
    _materials = std::move(materials);
    emit materialsChanged(_materials);
}

void MeshObject::setSubdivSettings(SubdivSettings settings) {
    auto self = *dynamicPointerCast<MeshObject>(sharedFromThis());
    auto change = makeShared<PropertyChange<MeshObject, SubdivSettings>>(self, std::move(settings), &MeshObject::subdivSettings, &MeshObject::setSubdivSettingsInternal);
    addChange(change);
}

void MeshObject::setSubdivSettingsInternal(SubdivSettings settings) {
    if (_subdivSettings == settings) {
        return;
    }
    _subdivSettings = std::move(settings);
    emit subdivSettingsChanged(settings);
}

void MeshObject::setMirrorSettings(MirrorSettings settings) {
    auto self = *dynamicPointerCast<MeshObject>(sharedFromThis());
    auto change = makeShared<PropertyChange<MeshObject, MirrorSettings>>(self, std::move(settings), &MeshObject::mirrorSettings, &MeshObject::setMirrorSettingsInternal);
    addChange(change);
}

void MeshObject::setMirrorSettingsInternal(MirrorSettings settings) {
    if (_mirrorSettings == settings) {
        return;
    }
    _mirrorSettings = std::move(settings);
    emit mirrorSettingsChanged(settings);
}

SP<Object> MeshObject::clone() const {
    auto cloned = makeShared<MeshObject>();
    // FIXME: object name is not copied
    *cloned->_mesh = *_mesh;
    return cloned;
}

void MeshObject::toJSON(nlohmann::json &json) const {
    Object::toJSON(json);
    throw std::runtime_error("TODO");
    // TODO
    // _shape->toJSON(json["shape"]);
}

void MeshObject::fromJSON(const nlohmann::json &json) {
    Object::fromJSON(json);
    throw std::runtime_error("TODO");
    // TODO
    //_shape->fomJSON(json["shape"]);
}

}
} // namespace Lattice
