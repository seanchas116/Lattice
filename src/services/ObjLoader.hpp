#pragma once
#include <string>
#include <vector>
#include "../support/Pointer.hpp"

namespace Lattice {
class MeshItem;
}

namespace Lattice::Services {

class ObjLoader final {
public:
    static std::vector<SP<MeshItem>> load(const std::string& filePath);
};

} // namespace Lattice
