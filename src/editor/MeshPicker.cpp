#include "MeshPicker.hpp"
#include "../support/ScopedTimer.hpp"
#include "../support/Distance.hpp"
#include "../support/Debug.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {

MeshPicker::MeshPicker(const SP<Document::Mesh> &mesh) : _mesh(mesh) {
}

std::optional<std::pair<SP<Document::MeshFace>, double> > MeshPicker::picKFace(const dmat4 &modelToWorld, const Camera &camera, dvec2 screenPos) const {
    Ray<float> ray = inverse(modelToWorld) * camera.worldMouseRay(screenPos);
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
                intersectings.insert({t, f});
            }
        }
    }
    if (intersectings.empty()) {
        return {};
    }
    auto nearest = intersectings.begin();
    return {{nearest->second, nearest->first}};
}

std::optional<std::pair<SP<Document::MeshVertex>, double> > MeshPicker::pickVertex(const dmat4 &modelToWorld, const Camera &camera, dvec2 screenPos, double distance) const {
    std::map<double, SP<Document::MeshVertex>> intersectings;

    for (auto& v : _mesh->vertices()) {
        auto [screenVertexPos, isInScreen] = camera.mapWorldToScreen((modelToWorld * vec4(v->position(), 1)).xyz);
        if (!isInScreen) { continue; }

        if (glm::distance(dvec2(screenVertexPos.xy), screenPos) <= distance) {
            intersectings.insert({screenVertexPos.z, v});
        }
    }

    if (intersectings.empty()) {
        return {};
    }
    auto nearest = intersectings.begin();
    return {{nearest->second, nearest->first}};
}

std::optional<std::pair<SP<Document::MeshEdge>, double> > MeshPicker::pickEdge(const dmat4 &modelToWorld, const Camera &camera, dvec2 screenPos, double distance) const {
    // TODO: Use screen space distance
    Ray<double> ray = inverse(modelToWorld) * camera.worldMouseRay(screenPos);

    std::map<double, SP<Document::MeshEdge>> intersectings;

    for (auto& [_, e] : _mesh->edges()) {
        dvec3 p0 = e->vertices()[0]->position();
        dvec3 p1 = e->vertices()[1]->position();
        Ray<double> edgeRay(p0, p1 - p0);
        RayRayDistance edgeMouseDistance(edgeRay, ray);
        if (edgeMouseDistance.distance <= distance && 0 <= edgeMouseDistance.t0 && edgeMouseDistance.t0 <= 1) {
            intersectings.insert({edgeMouseDistance.t1, e});
        }
    }

    if (intersectings.empty()) {
        return {};
    }
    auto nearest = intersectings.begin();
    return {{nearest->second, nearest->first}};
}

} // namespace Viewport
} // namespace Lattice
