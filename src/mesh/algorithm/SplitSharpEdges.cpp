#include "SplitSharpEdges.hpp"
#include <range/v3/algorithm/find.hpp>

namespace Lattice {
namespace Mesh {

void splitSharpEdges(Mesh &mesh) {
    for (auto v : mesh.vertices()) {
        int nSharpEdges = 0;
        for (auto e : mesh.edges(v)) {
            if (mesh.isSharp(e) || ranges::distance(mesh.faces(e)) >= 3) {
                ++nSharpEdges;
            }
        }
        if (nSharpEdges <= 1) {
            continue;
        }

        auto faces = mesh.faces(v) | ranges::to_vector;
        auto edges = mesh.edges(v) | ranges::to_vector;

        // 1. split faces into groups by sharp edges
        // 2. create separate vertices for each group

        std::vector<std::array<FaceHandle, 2>> faceConnections;
        for (auto e : edges) {
            if (mesh.isSharp(e)) {
                continue;
            }
            auto edgeFaces = mesh.faces(e) | ranges::to_vector;
            if (edgeFaces.size() != 2) {
                continue;
            }
            faceConnections.push_back({edgeFaces[0], edgeFaces[1]});
        }

        auto isConnected = [&](FaceHandle f0, FaceHandle f1) {
            for (auto connection : faceConnections) {
                if (connection[0] == f0 && connection[1] == f1) {
                    return true;
                }
                if (connection[0] == f1 && connection[1] == f0) {
                    return true;
                }
            }
            return false;
        };

        std::vector<std::vector<FaceHandle>> faceGroups;
        for (auto f : faces) {
            bool addedToGroup = false;
            for (auto& faceGroup : faceGroups) {
                bool connectedToGroup = false;
                for (auto existingFace : faceGroup) {
                    if (isConnected(f, existingFace)) {
                        connectedToGroup = true;
                        break;
                    }
                }
                if (connectedToGroup) {
                    faceGroup.push_back(f);
                    addedToGroup = true;
                    break;
                }
            }
            if (!addedToGroup) {
                faceGroups.push_back({f});
            }
        }

        for (auto it = faceGroups.begin() + 1; it != faceGroups.end(); ++it) {
            auto& faceGroup = *it;
            auto newVertex = mesh.addVertex(mesh.position(v));

            for (auto f : faceGroup) {
                auto vertices = mesh.vertices(f);
                auto vertexIndex = ranges::find(vertices, v) - vertices.begin();
                std::vector<UVPointHandle> uvPoints = mesh.uvPoints(f);
                uvPoints[vertexIndex] = mesh.addUVPoint(newVertex, mesh.uvPosition(uvPoints[vertexIndex]));
                auto material = mesh.material(f);

                mesh.removeFace(f);
                mesh.addFace(uvPoints, material);
            }
        }
    }
}

} // namespace Mesh
} // namespace Lattice
