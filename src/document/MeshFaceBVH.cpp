#include "MeshFaceBVH.hpp"

using namespace glm;

namespace Lattice {
namespace Document {

MeshFaceBVH::MeshFaceBVH(const SP<Mesh> &mesh) {
}

std::optional<std::pair<SP<MeshFace>, float>> MeshFaceBVH::pick(const Ray<float> &ray) const {
    return nodes[0].pick(ray);
}

std::optional<std::pair<SP<MeshFace>, float>> MeshFaceBVH::Node::pick(const Ray<float> &ray) const {
    if (!boundingBox.intersects(ray)) {
        return {};
    }
    if (faces.empty()) {
        auto order = ray.direction[splitAxis] > 0 ? std::array{0, 1} : std::array{1, 0};
        for (int i : order) {
            auto result = children[size_t(i)]->pick(ray);
            if (result) { return result; }
        }
        return {};
    } else {
        // leaf node
        std::map<float, SP<Document::MeshFace>> intersectings;
        for (auto& f : faces) {
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
}

} // namespace Document
} // namespace Lattice
