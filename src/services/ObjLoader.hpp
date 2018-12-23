#pragma once
#include <string>
#include "../support/Pointer.hpp"

namespace Lattice {

class MeshItem;

class ObjLoader {
public:
    static SP<MeshItem> load(const std::string& filePath);
};

} // namespace Lattice
