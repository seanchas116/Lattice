#include "MeshFragment.hpp"

using namespace glm;

namespace Lattice {
namespace Mesh {

glm::vec3 MeshFragment::medianPosition() const {
    if (vertices.empty()) {
        return vec3(0);
    }

    vec3 minPos(std::numeric_limits<float>::infinity());
    vec3 maxPos(-std::numeric_limits<float>::infinity());

    for (auto& v : vertices) {
        auto p = v->position();
        minPos = min(minPos, p);
        maxPos = max(maxPos, p);
    }

    return (minPos + maxPos) * 0.5f;
}

std::unordered_set<SP<Edge> > MeshFragment::edges() const {
    std::unordered_map<SP<Edge>, size_t> edgeCounts;

    for (auto& v : vertices) {
        for (auto& e : v->edges()) {
            if (edgeCounts.find(e->sharedFromThis()) != edgeCounts.end()) {
                ++edgeCounts[e->sharedFromThis()];
            } else {
                edgeCounts[e->sharedFromThis()] = 1;
            }
        }
    }
    std::unordered_set<SP<Edge>> edges;

    for (auto& [edge, count] : edgeCounts) {
        if (count == 2) {
            edges.insert(edge);
        }
    }

    return edges;
}

std::unordered_set<SP<Face> > MeshFragment::faces() const {
    std::unordered_map<SP<Face>, size_t> faceCounts;

    for (auto& v : vertices) {
        for (auto& f : v->faces()) {
            if (faceCounts.find(f->sharedFromThis()) != faceCounts.end()) {
                ++faceCounts[f->sharedFromThis()];
            } else {
                faceCounts[f->sharedFromThis()] = 1;
            }
        }
    }
    std::unordered_set<SP<Face>> faces;

    for (auto& [face, count] : faceCounts) {
        if (count == face->vertices().size()) {
            faces.insert(face);
        }
    }

    return faces;
}

}
}
