#include "Extrude.hpp"

namespace Lattice {
namespace Mesh {

std::vector<VertexHandle> extrude(Mesh &mesh, const std::vector<VertexHandle> &vertices) {
    std::vector<VertexHandle> newVertices;
    std::unordered_map<VertexHandle, UVPointHandle> vertexToUV;
    std::unordered_map<UVPointHandle, UVPointHandle> oldToNewUVPoints;

    auto edges = mesh.edges(vertices);
    auto faces = mesh.faces(vertices);

    std::unordered_set<EdgeHandle> openEdges;
    for (auto&& edge : edges) {
        int faceCount = 0;
        for (auto&& face : mesh.faces(edge)) {
            if (faces.find(face) != faces.end()) {
                ++faceCount;
            }
        }
        if (faceCount <= 1) {
            openEdges.insert(edge);
        }
    }

    for (auto&& vertex : vertices) {
        auto uvPoint = mesh.uvPoints(vertex).front(); // TODO: find best uv
        auto newVertex = mesh.addVertex(mesh.position(vertex));
        auto newUVPoint = mesh.addUVPoint(newVertex, mesh.uvPosition(uvPoint));
        mesh.addEdge({vertex, newVertex});
        vertexToUV.insert({vertex, uvPoint});
        vertexToUV.insert({newVertex, newUVPoint});
        oldToNewUVPoints.insert({uvPoint, newUVPoint});
        newVertices.push_back(newVertex);
    }

    for (auto& edge : edges) {
        auto uv0 = oldToNewUVPoints.at(vertexToUV.at(mesh.vertices(edge)[0]));
        auto uv1 = oldToNewUVPoints.at(vertexToUV.at(mesh.vertices(edge)[1]));
        mesh.addEdge({mesh.vertex(uv0), mesh.vertex(uv1)});
    }

    for (auto& openEdge : openEdges) {
        bool isReverse = true;
        MaterialHandle material;

        for (auto& face : mesh.faces(openEdge)) {
            if (faces.find(face) != faces.end()) {
                material = mesh.material(face);
                continue;
            }

            for (size_t i = 0; i < mesh.vertices(face).size(); ++i) {
                size_t i2 = (i + 1) % mesh.vertices(face).size();
                if (mesh.vertices(face)[i] == mesh.vertices(openEdge)[0] && mesh.vertices(face)[i2] == mesh.vertices(openEdge)[1]) {
                    isReverse = false;
                    break;
                }
            }
        }

        auto uv0 = vertexToUV.at(mesh.vertices(openEdge)[0]);
        auto uv1 = vertexToUV.at(mesh.vertices(openEdge)[1]);
        auto uv2 = oldToNewUVPoints.at(uv1);
        auto uv3 = oldToNewUVPoints.at(uv0);

        if (isReverse) {
            mesh.addFace({uv0, uv1, uv2, uv3}, material);
        } else {
            mesh.addFace({uv3, uv2, uv1, uv0}, material);
        }
    }

    for (auto& face : faces) {
        std::vector<UVPointHandle> newUVPoints;
        for (auto& uv : mesh.uvPoints(face)) {
            auto newUV = oldToNewUVPoints.at(uv);
            newUVPoints.push_back(newUV);
        }
        mesh.addFace(newUVPoints, mesh.material(face));
        mesh.removeFace(face);
    }

    return newVertices;
}

} // namespace Mesh
} // namespace Lattice
