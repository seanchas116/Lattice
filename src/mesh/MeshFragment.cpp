#include "MeshFragment.hpp"

using namespace glm;

namespace Lattice {
namespace OldMesh {

bool MeshFragment::contains(const MeshFragment &other) const {
    for (auto& v : other.vertices) {
        if (vertices.find(v) == vertices.end()) {
            return false;
        }
    }
    return true;
}

glm::dvec3 MeshFragment::medianPosition() const {
    if (vertices.empty()) {
        return dvec3(0);
    }

    dvec3 minPos(std::numeric_limits<double>::infinity());
    dvec3 maxPos(-std::numeric_limits<double>::infinity());

    for (auto& v : vertices) {
        auto p = v->position();
        minPos = min(minPos, p);
        maxPos = max(maxPos, p);
    }

    return (minPos + maxPos) * 0.5;
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
