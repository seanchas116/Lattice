#include "MeshPicker.hpp"
#include "../support/ScopedTimer.hpp"
#include "../support/Distance.hpp"
#include "../support/Debug.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {

namespace {

// same as DrawLine.geom
bool mapLineToViewport(const mat4& P, vec2 viewportSize, float zNear, vec4 p0_cameraSpace, vec4 p1_cameraSpace, vec3& p0_viewportSpace, vec3& p1_viewportSpace) {
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

    vec2 p0 = (p0_clipSpace.xy() / p0_clipSpace.w + 1.f) * (0.5f * viewportSize);
    vec2 p1 = (p1_clipSpace.xy() / p1_clipSpace.w + 1.f) * (0.5f * viewportSize);
    float d0 = p0_clipSpace.z / p0_clipSpace.w * 0.5f + 0.5f;
    float d1 = p1_clipSpace.z / p1_clipSpace.w * 0.5f + 0.5f;

    p0_viewportSpace = vec3(p0, d0);
    p1_viewportSpace = vec3(p1, d1);
    return true;
}

}

MeshPicker::MeshPicker(const SP<Mesh::Mesh> &mesh) : _mesh(mesh) {
}

Opt<std::pair<SP<Mesh::Face>, double> > MeshPicker::pickFace(const dmat4 &modelToWorld, const SP<Camera> &camera, dvec2 viewportPos) const {
    Ray<float> ray = inverse(modelToWorld) * camera->worldMouseRay(viewportPos);
    // TODO: Use Bounding Volume Hierarchy to do faster
    //ScopedTimer timer("MeshPicker::pickFace");
    std::map<double, SP<Mesh::Face>> intersectings;
    for (auto& [_, f] : _mesh->faces()) {
        auto v0 = f->vertices()[0]->position();
        for (uint32_t i = 2; i < uint32_t(f->vertices().size()); ++i) {
            auto v1 = f->vertices()[i - 1]->position();
            auto v2 = f->vertices()[i]->position();

            auto [intersects, t] = ray.intersectsTriangle({v0, v1, v2});
            if (intersects) {
                dvec3 intersectingPos = ray.at(t);
                auto [intersectingPosViewport, isInViewport] = camera->mapWorldToViewport((modelToWorld * dvec4(intersectingPos, 1)).xyz());
                intersectings.insert({intersectingPosViewport.z, f});
            }
        }
    }
    if (intersectings.empty()) {
        return {};
    }
    auto nearest = intersectings.begin();
    return {{nearest->second, nearest->first}};
}

Opt<std::pair<SP<Mesh::Vertex>, double> > MeshPicker::pickVertex(const dmat4 &modelToWorld, const SP<Camera> &camera, dvec2 viewportPos, double distance) const {
    std::map<double, SP<Mesh::Vertex>> intersectings;

    for (auto& v : _mesh->vertices()) {
        auto [viewportVertexPos, isInViewport] = camera->mapWorldToViewport((modelToWorld * vec4(v->position(), 1)).xyz());
        if (!isInViewport) { continue; }

        if (glm::distance(viewportVertexPos.xy(), viewportPos) <= distance) {
            intersectings.insert({viewportVertexPos.z, v});
        }
    }

    if (intersectings.empty()) {
        return {};
    }
    auto nearest = intersectings.begin();
    return {{nearest->second, nearest->first}};
}

Opt<std::pair<SP<Mesh::Edge>, double> > MeshPicker::pickEdge(const dmat4 &modelToWorld, const SP<Camera> &camera, dvec2 viewportPos, double distance) const {
    std::map<double, SP<Mesh::Edge>> intersectings;

    for (auto& [_, e] : _mesh->edges()) {
        vec4 p0_modelSpace = vec4(e->vertices()[0]->position(), 1);
        vec4 p1_modelSpace = vec4(e->vertices()[1]->position(), 1);
        vec4 p0_cameraSpace = camera->worldToCameraMatrix() * modelToWorld * p0_modelSpace;
        vec4 p1_cameraSpace = camera->worldToCameraMatrix() * modelToWorld * p1_modelSpace;
        vec3 p0_viewportSpace;
        vec3 p1_viewportSpace;
        bool ok = mapLineToViewport(camera->cameraToViewportMatrix(), camera->viewportSize(), camera->zNear(), p0_cameraSpace, p1_cameraSpace, p0_viewportSpace, p1_viewportSpace);
        if (!ok) {
            continue;
        }

        vec2 ab = p1_viewportSpace.xy() - p0_viewportSpace.xy();
        vec2 w = vec2(viewportPos) - p0_viewportSpace.xy();
        vec2 v = normalize(ab);
        float d = abs(cross(vec3(w, 0), vec3(v, 0)).z);
        float t = dot(ab, w) / dot(ab, ab);

        if (d <= distance && 0 <= t && t <= 1) {
            float depth =  mix(p0_viewportSpace.z, p1_viewportSpace.z, t);
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
