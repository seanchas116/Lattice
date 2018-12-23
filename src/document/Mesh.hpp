#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

namespace Lattice {

class Mesh {
public:
    struct VertexInfo {
        glm::vec3 position;
    };

    struct EdgeInfo {
        // TODO
    };

    struct FaceInfo {
        // TODO
    };

    Mesh();

private:

    std::vector<VertexInfo> _vertices;
    std::unordered_map<std::pair<size_t, size_t>, EdgeInfo> _edges;
    std::unordered_map<std::vector<size_t>, FaceInfo> _faces;
};

} // namespace Lattice

