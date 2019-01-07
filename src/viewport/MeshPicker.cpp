#include "MeshPicker.hpp"
#include "../support/ScopedTimer.hpp"

namespace Lattice {
namespace Viewport {

MeshPicker::MeshPicker(const SP<Document::Mesh> &mesh) : _mesh(mesh) {
}

std::pair<SP<Document::MeshFace>, double> MeshPicker::picKFace(const Ray<double> &ray) const {
    ScopedTimer timer("MeshPicker::pickFace");
    // TODO: Use Bounding Volume Hierarchy to do faster
    // TODO: sort intersectings
    for (auto& [_, f] : _mesh->faces()) {
        auto v0 = f->vertices()[0]->position();
        for (uint32_t i = 2; i < uint32_t(f->vertices().size()); ++i) {
            auto v1 = f->vertices()[i - 1]->position();
            auto v2 = f->vertices()[i]->position();

            auto [intersects, t] = ray.intersectsTriangle({v0, v1, v2});
            if (intersects) {
                return {f, t};
            }
        }
    }
    return {{}, 0};
}

std::pair<SP<Document::MeshVertex>, double> MeshPicker::pickVertex(const Ray<double> &ray, double distance) const {
    Q_UNUSED(ray);
    Q_UNUSED(distance);
    return {}; // TODO
}

std::pair<SP<Document::MeshEdge>, double> MeshPicker::pickEdge(const Ray<double> &ray, double distance) const {
    Q_UNUSED(ray);
    Q_UNUSED(distance);
    return {}; // TODO
}

} // namespace Viewport
} // namespace Lattice
