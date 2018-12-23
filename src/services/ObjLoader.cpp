#include "ObjLoader.hpp"
#include "../document/MeshItem.hpp"
#include <tiny_obj_loader.h>

namespace Lattice {

SP<MeshItem> ObjLoader::load(const std::string &filePath) {
    auto meshItem = std::make_shared<MeshItem>();
    return meshItem;
}

} // namespace Lattice
