#include "MeshData.hpp"
#include "Mesh.hpp"
#include <nlohmann/json.hpp>

namespace {

template <typename T>
std::string toDataString(const std::vector<T>& data) {
    std::string dataString(data.size() * sizeof(T), 0);
    memcpy(dataString.data(), data.data(), dataString.size());
    return dataString;
}

template <typename T>
std::vector<T> fromDataString(const std::string& dataString) {
    std::vector<T> data(dataString.size() / sizeof(T));
    memcpy(data.data(), dataString.data(), data.size() * sizeof(T));
    return data;
}

}

namespace Lattice {
namespace Mesh {

MeshData::MeshData(const Mesh &origMesh) {
    auto mesh = origMesh.collectGarbage();

    for (auto v : mesh.vertices()) {
        vertexPositionArray.push_back(mesh.position(v));
        vertexSelectedArray.push_back(mesh.isSelected(v));
        vertexCornerArray.push_back(mesh.corner(v));
    }
    for (auto uv : mesh.uvPoints()) {
        uvPositionArray.push_back(mesh.uvPosition(uv));
        uvVertexArray.push_back(mesh.vertex(uv).index);
    }
    for (auto e : mesh.edges()) {
        auto& vertices = mesh.vertices(e);
        edgeVerticesArray.push_back({vertices[0].index, vertices[1].index});
        edgeSmoothArray.push_back(mesh.isSmooth(e));
        edgeCreaseArray.push_back(mesh.crease(e));
    }
    for (auto f : mesh.faces()) {
        auto& uvPoints = mesh.uvPoints(f);
        faceVertexCountArray.push_back(uvPoints.size());
        faceMaterialArray.push_back(mesh.material(f).index);
        for (auto uv : uvPoints) {
            faceUVPointArray.push_back(uv.index);
        }
    }
}

Mesh MeshData::toMesh() const {
    Mesh mesh;

    auto vertexCount = vertexPositionArray.size();
    for (size_t i = 0; i < vertexCount; ++i) {
        auto v = mesh.addVertex(vertexPositionArray[i]);
        mesh.setSelected(v, vertexSelectedArray[i]);
        mesh.setCorner(v, vertexCornerArray[i]);
    }

    auto uvPointCount = uvPositionArray.size();
    for (size_t i = 0; i < uvPointCount; ++i) {
        auto pos = uvPositionArray[i];
        mesh.addUVPoint(VertexHandle(uvVertexArray[i]), pos);
    }

    auto edgeCount = edgeVerticesArray.size();
    for (size_t i = 0; i < edgeCount; ++i) {
        auto vertices = edgeVerticesArray[i];
        auto edge = mesh.addEdge({VertexHandle(vertices[0]), VertexHandle(vertices[1])});
        mesh.setSmooth(edge, edgeSmoothArray[i]);
        mesh.setCrease(edge, edgeCreaseArray[i]);
    }

    auto faceCount = faceVertexCountArray.size();
    size_t uvPointOffset = 0;

    for (size_t i = 0; i < faceCount; ++i) {
        auto vertexCount = faceVertexCountArray[i];
        auto material = MaterialHandle(faceMaterialArray[i]);

        std::vector<UVPointHandle> uvPoints;
        for (size_t j = 0; j < vertexCount; ++j) {
            uvPoints.push_back(UVPointHandle(faceUVPointArray[uvPointOffset + j]));
        }
        uvPointOffset += vertexCount;
        mesh.addFace(uvPoints, material);
    }

    return mesh;
}

void to_json(nlohmann::json &json, const MeshData &meshData) {
    json["vertex"]["position"] = toDataString(meshData.vertexPositionArray);
    json["vertex"]["selected"] = toDataString(meshData.vertexSelectedArray);
    json["vertex"]["corner"] = toDataString(meshData.vertexCornerArray);

    json["uvPoint"]["position"] = toDataString(meshData.uvPositionArray);
    json["uvPoint"]["vertex"] = toDataString(meshData.uvVertexArray);

    json["edge"]["smooth"] = toDataString(meshData.edgeSmoothArray);
    json["edge"]["vertices"] = toDataString(meshData.edgeVerticesArray);
    json["edge"]["crease"] = toDataString(meshData.edgeCreaseArray);

    json["face"]["material"] = toDataString(meshData.faceMaterialArray);
    json["face"]["vertexCount"] = toDataString(meshData.faceVertexCountArray);
    json["face"]["uvPoint"] = toDataString(meshData.faceUVPointArray);
}

void from_json(const nlohmann::json &json, MeshData &meshData) {
    meshData.vertexPositionArray = fromDataString<glm::vec3>(json["vertex"]["position"]);
    meshData.vertexSelectedArray = fromDataString<uint8_t>(json["vertex"]["selected"]);
    meshData.vertexCornerArray = fromDataString<float>(json["vertex"]["corner"]);

    meshData.uvPositionArray = fromDataString<glm::vec2>(json["uvPoint"]["position"]);
    meshData.uvVertexArray = fromDataString<int32_t>(json["uvPoint"]["vertex"]);

    meshData.edgeSmoothArray = fromDataString<uint8_t>(json["edge"]["smooth"]);
    meshData.edgeCreaseArray = fromDataString<float>(json["edge"]["crease"]);
    meshData.edgeVerticesArray = fromDataString<std::array<int32_t, 2>>(json["edge"]["vertices"]);

    meshData.faceMaterialArray = fromDataString<int32_t>(json["face"]["material"]);
    meshData.faceVertexCountArray = fromDataString<int32_t>(json["face"]["vertexCount"]);
    meshData.faceUVPointArray = fromDataString<int32_t>(json["face"]["uvPoint"]);
}

} // namespace Mesh
} // namespace Lattice
