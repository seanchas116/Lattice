#include "ObjLoader.hpp"
#include "../document/MeshItem.hpp"
#include <tiny_obj_loader.h>
#include <QtDebug>

namespace Lattice {

std::vector<SP<MeshItem>> ObjLoader::load(const std::string &filePath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> objShapes;
    std::vector<tinyobj::material_t> objMaterials;

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &objShapes, &objMaterials, &err, filePath.c_str());

    if (!err.empty()) {
        qWarning() << err.c_str();
    }
    if (!ret) {
        return {};
    }

    return {}; // TODO
}

} // namespace Lattice
