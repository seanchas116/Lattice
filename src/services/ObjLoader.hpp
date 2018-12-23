#pragma once
#include <string>
#include "../support/Pointer.hpp"

namespace Lattice {

class MeshShape;

class ObjLoader {
public:
    static SP<MeshShape> load(const std::string& filePath);
};

} // namespace Lattice
