#include "FlipFace.hpp"
#include <range/v3/view/reverse.hpp>

namespace Lattice {
namespace Mesh {

FaceHandle flipFace(Mesh &mesh, FaceHandle face) {
    auto reverseUVPoints = mesh.uvPoints(face) | ranges::view::reverse | ranges::to_vector;
    auto newFace = mesh.addFace(reverseUVPoints, mesh.material(face));
    mesh.removeFace(face);
    return newFace;
}

} // namespace Mesh
} // namespace Lattice
