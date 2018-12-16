#include "Resource.hpp"

#include <cmrc/cmrc.hpp>

CMRC_DECLARE(LatticeResources);

namespace Lattice {

std::string readResource(const char *path) {
    auto file = cmrc::LatticeResources::get_filesystem().open(path);
    return std::string(file.begin(), file.end());
}

}
