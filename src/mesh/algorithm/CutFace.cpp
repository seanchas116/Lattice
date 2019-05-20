#include "CutFace.hpp"
#include <range/v3/algorithm/find_if.hpp>

namespace Lattice {
namespace Mesh {

void CutFace::perform(Mesh &mesh) {
    std::unordered_set<FaceHandle> facesToCheckSplit;
    for (auto v : mesh.vertices(edge)) {
        for (auto face : mesh.faces(v)) {
            facesToCheckSplit.insert(face);
        }
    }

    std::vector<FaceHandle> facesToRemove;
    std::vector<std::tuple<std::vector<UVPointHandle>, std::vector<UVPointHandle>, MaterialHandle>> faceAdditions;

    // split faces that includes newly added edge
    for (auto& face : facesToCheckSplit) {
        auto& faceUVPoints = mesh.uvPoints(face);
        auto uv0It = ranges::find_if(faceUVPoints, [&](auto& uv) { return mesh.vertex(uv) == mesh.vertices(edge)[0]; });
        auto uv1It = ranges::find_if(faceUVPoints, [&](auto& uv) { return mesh.vertex(uv) == mesh.vertices(edge)[1]; });
        if (uv0It != faceUVPoints.end() && uv1It != faceUVPoints.end()) {
            // face includes edge
            if (uv1It < uv0It) {
                std::swap(uv0It, uv1It);
            }

            std::vector<UVPointHandle> uvPoints0;
            uvPoints0.insert(uvPoints0.end(), uv1It, faceUVPoints.end());
            uvPoints0.insert(uvPoints0.end(), faceUVPoints.begin(), uv0It + 1);
            std::vector<UVPointHandle> uvPoints1(uv0It, uv1It + 1);

            faceAdditions.push_back({uvPoints0, uvPoints1, mesh.material(face)});
            facesToRemove.push_back(face);
        }
    }
    for (auto& [uvPoints0, uvPoints1, material] : faceAdditions) {
        mesh.addFace(uvPoints0, material);
        mesh.addFace(uvPoints1, material);
    }
    for (auto& f : facesToRemove) {
        mesh.removeFace(f);
    }

}

} // namespace Mesh
} // namespace Lattice
