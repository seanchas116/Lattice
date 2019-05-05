#include "MeshData.hpp"
#include "Mesh.hpp"
#include <nlohmann/json.hpp>

namespace {

template <typename T>
std::string toDataString(const std::vector<T>& data) {
    size_t size = data.size() * sizeof(T);
    std::string dataString(size, 0);
    memcpy(dataString.data(), data.data(), size);
    return dataString;
}

}

namespace Lattice {
namespace Mesh {

MeshData::MeshData(const Mesh &mesh) {
    for (auto v : mesh.vertices()) {
        vertexPositionArray.push_back(mesh.position(v));
        vertexSelectedArray.push_back(mesh.isSelected(v));
    }
    for (auto uv : mesh.uvPoints()) {
        uvPositionArray.push_back(mesh.uvPosition(uv));
        uvVertexArray.push_back(mesh.vertex(uv).index);
    }
    for (auto e : mesh.edges()) {
        auto& vertices = mesh.vertices(e);
        edgeVerticesArray.push_back({vertices[0].index, vertices[1].index});
        edgeSmoothArray.push_back(mesh.isSmooth(e));
    }
    for (auto f : mesh.faces()) {
        auto& uvPoints = mesh.uvPoints(f);
        faceVertexCountArray.push_back(uvPoints.size());
        faceMaterialArray.push_back(mesh.material(f));
        for (auto uv : uvPoints) {
            faceUVPointArray.push_back(uv.index);
        }
    }
}

void to_json(nlohmann::json &json, const MeshData &meshData) {
    json["vertex"]["position"] = toDataString(meshData.vertexPositionArray);
    json["vertex"]["selected"] = toDataString(meshData.vertexSelectedArray);
    json["uvPoint"]["position"] = toDataString(meshData.uvPositionArray);
    json["uvPoint"]["vertex"] = toDataString(meshData.uvVertexArray);
    json["edge"]["vertices"] = toDataString(meshData.edgeVerticesArray);
    json["edge"]["smooth"] = toDataString(meshData.edgeSmoothArray);
    json["face"]["vertexCount"] = toDataString(meshData.faceVertexCountArray);
    json["face"]["material"] = toDataString(meshData.faceMaterialArray);
    json["face"]["uvPoint"] = toDataString(meshData.faceUVPointArray);
}

} // namespace Mesh
} // namespace Lattice
