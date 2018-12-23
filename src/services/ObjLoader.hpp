#pragma once
#include <string>
#include <vector>
#include "../support/Pointer.hpp"

namespace Lattice {

class MeshItem;

class ObjLoader {
public:
    static std::vector<SP<MeshItem>> load(const std::string& filePath);
};

} // namespace Lattice
