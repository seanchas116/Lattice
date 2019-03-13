#include "MeshSelection.hpp"

using namespace glm;

namespace Lattice {
namespace Document {

glm::vec3 MeshSelection::medianPosition() const {
    if (vertices.empty()) {
        return vec3(0);
    }

    vec3 minPos(INFINITY);
    vec3 maxPos(-INFINITY);

    for (auto& v : vertices) {
        auto p = v->position();
        minPos = min(minPos, p);
        maxPos = max(maxPos, p);
    }

    return (minPos + maxPos) / 2.f;
}

std::unordered_set<SP<Mesh::Edge> > MeshSelection::edges() const {
    std::unordered_map<SP<Mesh::Edge>, size_t> edgeCounts;

    for (auto& v : vertices) {
        for (auto& e : v->edges()) {
            if (edgeCounts.find(e->sharedFromThis()) != edgeCounts.end()) {
                ++edgeCounts[e->sharedFromThis()];
            } else {
                edgeCounts[e->sharedFromThis()] = 1;
            }
        }
    }
    std::unordered_set<SP<Mesh::Edge>> edges;

    for (auto& [edge, count] : edgeCounts) {
        if (count == 2) {
            edges.insert(edge);
        }
    }

    return edges;
}

std::unordered_set<SP<Mesh::Face> > MeshSelection::faces() const {
    std::unordered_map<SP<Mesh::Face>, size_t> faceCounts;

    for (auto& v : vertices) {
        for (auto& f : v->faces()) {
            if (faceCounts.find(f->sharedFromThis()) != faceCounts.end()) {
                ++faceCounts[f->sharedFromThis()];
            } else {
                faceCounts[f->sharedFromThis()] = 1;
            }
        }
    }
    std::unordered_set<SP<Mesh::Face>> faces;

    for (auto& [face, count] : faceCounts) {
        if (count == face->vertices().size()) {
            faces.insert(face);
        }
    }

    return faces;
}

} // namespace Document
} // namespace Lattice
