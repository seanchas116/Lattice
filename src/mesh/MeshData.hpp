#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <nlohmann/json_fwd.hpp>

namespace Lattice {
namespace Mesh {

class Mesh;

struct MeshData {
    std::vector<glm::vec3> vertexPositionArray;
    std::vector<uint8_t> vertexSelectedArray;

    std::vector<glm::vec2> uvPositionArray;
    std::vector<uint32_t> uvVertexArray;

    std::vector<uint8_t> edgeSmoothArray;
    std::vector<std::array<uint32_t, 2>> edgeVerticesArray;

    std::vector<uint32_t> faceMaterialArray;
    std::vector<uint32_t> faceVertexCountArray;
    std::vector<uint32_t> faceUVPointArray;

    explicit MeshData(const Mesh &mesh);
    Mesh toMesh() const;
};

void to_json(nlohmann::json& json, const MeshData& meshData);
void from_json(const nlohmann::json& json, MeshData& meshData);

} // namespace Mesh
} // namespace Lattice
