#include "MeshPicker.hpp"
#include "../support/ScopedTimer.hpp"
#include "../support/Distance.hpp"

using namespace glm;

namespace Lattice {
namespace Viewport {

MeshPicker::MeshPicker(const SP<Document::Mesh> &mesh) : _mesh(mesh) {
}

std::pair<SP<Document::MeshFace>, double> MeshPicker::picKFace(const Ray<double> &ray) const {
    // TODO: Use Bounding Volume Hierarchy to do faster
    ScopedTimer timer("MeshPicker::pickFace");
    std::map<float, SP<Document::MeshFace>> intersectings;
    for (auto& [_, f] : _mesh->faces()) {
        auto v0 = f->vertices()[0]->position();
        for (uint32_t i = 2; i < uint32_t(f->vertices().size()); ++i) {
            auto v1 = f->vertices()[i - 1]->position();
            auto v2 = f->vertices()[i]->position();

            auto [intersects, t] = ray.intersectsTriangle({v0, v1, v2});
            if (intersects) {
                intersectings[t] = f;
            }
        }
    }
    if (intersectings.empty()) {
        return {};
    }
    auto nearest = intersectings.begin();
    return {nearest->second, nearest->first};
}

std::pair<SP<Document::MeshVertex>, double> MeshPicker::pickVertex(const Ray<double> &ray, double distance) const {
    Q_UNUSED(ray);
    Q_UNUSED(distance);
    return {}; // TODO
}

std::pair<SP<Document::MeshEdge>, double> MeshPicker::pickEdge(const Ray<double> &ray, double distance) const {
    std::map<double, SP<Document::MeshEdge>> intersectings;
    for (auto& [_, e] : _mesh->edges()) {
        dvec3 p0 = e->vertices()[0]->position();
        dvec3 p1 = e->vertices()[1]->position();
        Ray<double> edgeRay(p0, p1 - p0);
        RayRayDistance edgeMouseDistance(edgeRay, ray);
        if (edgeMouseDistance.distance <= distance && 0 <= edgeMouseDistance.t0 && edgeMouseDistance.t0 <= 1) {
            intersectings[edgeMouseDistance.t1] = e;
        }
    }

    if (intersectings.empty()) {
        return {};
    }
    auto nearest = intersectings.begin();
    return {nearest->second, nearest->first};
}

} // namespace Viewport
} // namespace Lattice
