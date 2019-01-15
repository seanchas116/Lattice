#include "MeshPicker.hpp"
#include "../support/ScopedTimer.hpp"
#include "../support/Distance.hpp"
#include "../support/Debug.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {

namespace {

// same as DrawLine.geom
bool mapLineToScreen(const mat4& P, vec2 viewportSize, float zNear, vec4 p0_cameraSpace, vec4 p1_cameraSpace, vec3& p0_screenSpace, vec3& p1_screenSpace) {
    // Don't render lines behind camera
    if (p0_cameraSpace.z > -zNear && p1_cameraSpace.z > -zNear) {
        return false;
    }

    // Make sure p0.z < p1.z
    if (p1_cameraSpace.z < p0_cameraSpace.z) {
        vec4 tmp = p0_cameraSpace;
        p0_cameraSpace = p1_cameraSpace;
        p1_cameraSpace = tmp;
    }

    // Clip line using near plane
    if (p1_cameraSpace.z > -zNear) {
        vec4 pnear_cameraSpace = mix(p0_cameraSpace, p1_cameraSpace, (-zNear - p0_cameraSpace.z) / (p1_cameraSpace.z - p0_cameraSpace.z));
        p1_cameraSpace = pnear_cameraSpace;
    }

    vec4 p0_clipSpace = P * p0_cameraSpace;
    vec4 p1_clipSpace = P * p1_cameraSpace;

    vec2 p0 = (vec2(p0_clipSpace.xy) / p0_clipSpace.w + 1.f) * (0.5f * viewportSize);
    vec2 p1 = (vec2(p1_clipSpace.xy) / p1_clipSpace.w + 1.f) * (0.5f * viewportSize);
    float d0 = p0_clipSpace.z / p0_clipSpace.w;
    float d1 = p1_clipSpace.z / p1_clipSpace.w;

    p0_screenSpace = vec3(p0, d0);
    p1_screenSpace = vec3(p1, d1);
    return true;
}

}

MeshPicker::MeshPicker(const SP<Document::Mesh> &mesh) : _mesh(mesh) {
}

std::optional<std::pair<SP<Document::MeshFace>, double> > MeshPicker::pickFace(const dmat4 &modelToWorld, const Camera &camera, dvec2 screenPos) const {
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
    std::map<double, SP<Document::MeshEdge>> intersectings;

    for (auto& [_, e] : _mesh->edges()) {
        vec4 p0_modelSpace = vec4(e->vertices()[0]->position(), 1);
        vec4 p1_modelSpace = vec4(e->vertices()[1]->position(), 1);
        vec4 p0_cameraSpace = camera.worldToCameraMatrix() * modelToWorld * p0_modelSpace;
        vec4 p1_cameraSpace = camera.worldToCameraMatrix() * modelToWorld * p1_modelSpace;
        vec3 p0_screenSpace;
        vec3 p1_screenSpace;
        bool ok = mapLineToScreen(camera.cameraToScreenMatrix(), camera.viewSize(), camera.zNear(), p0_cameraSpace, p1_cameraSpace, p0_screenSpace, p1_screenSpace);
        if (!ok) {
            continue;
        }

        vec2 ab = p1_screenSpace.xy - p0_screenSpace.xy;
        vec2 w = vec2(screenPos) - p0_screenSpace.xy;
        vec2 v = normalize(ab);
        float d = abs(cross(vec3(w, 0), vec3(v, 0)).z);
        float t = dot(ab, w) / dot(ab, ab);

        if (d <= distance && 0 <= t && t <= 1) {
            float depth =  mix(p0_screenSpace.z, p1_screenSpace.z, t);
            intersectings.insert({depth, e});
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
