#pragma once

#include <glm/glm.hpp>
#include <tuple>
#include <array>

namespace Lattice {

template <typename T>
class Ray final {
public:
    Ray() = default;
    Ray(glm::tvec3<T> origin, glm::tvec3<T> direction) : origin(origin), direction(direction) {}

    template <typename U>
    Ray(const Ray<U>& other) : origin(other.origin), direction(other.direction) {}

    std::tuple<bool, float> intersectsTriangle(const std::array<glm::tvec3<T>, 3>& triangle) const {
        // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
        using namespace glm;
        const T EPSILON = 0.0000001;
        auto [vertex0, vertex1, vertex2] = triangle;
        tvec3<T> edge1, edge2, h, s, q;
        T a,f,u,v;
        edge1 = vertex1 - vertex0;
        edge2 = vertex2 - vertex0;
        h = cross(direction, edge2);
        a = dot(edge1, h);
        if (a > -EPSILON && a < EPSILON) {
            return {false, 0};    // This ray is parallel to this triangle.
        }
        f = 1/a;
        s = origin - vertex0;
        u = f * (dot(s, h));
        if (u < 0.0 || u > 1.0) {
            return {false, 0};
        }
        q = cross(s, edge1);
        v = f * dot(direction, q);
        if (v < 0.0 || u + v > 1.0) {
            return {false, 0};
        }
        // At this stage we can compute t to find out where the intersection point is on the line.
        T t = f * dot(edge2, q);
        if (t > EPSILON) {
            return {true, t};
        } else {
            return {false, 0};
        }
    }

    glm::tvec3<T> origin {0};
    glm::tvec3<T> direction {0};
};

template <typename T>
class RayPointDistance final {
public:
    RayPointDistance(const Ray<T>& l, const glm::tvec3<T> p) {
        // http://geomalgorithms.com/a02-_lines.html#Distance-to-Infinite-Line
        using namespace glm;
        tvec3<T> w = p - l.origin;
        tvec3<T> v = glm::normalize(l.direction);
        distance = length(cross(v, w));
        t = dot(v, w);
    }
    T t;
    T distance;
};

template <typename T>
class RayRayDistance final {
public:
    RayRayDistance(const Ray<T>& l0, const Ray<T>& l1) {
        // http://geomalgorithms.com/a07-_distance.html
        using namespace glm;
        tvec3<T> u = l0.direction;
        tvec3<T> v = l1.direction;
        tvec3<T> w0 = l0.origin - l1.origin;

        T a = dot(u, u);
        T b = dot(u, v);
        T c = dot(v, v);
        T d = dot(u, w0);
        T e = dot(v, w0);

        T denom = a * c - b * b;

        t0 = (b * e - c * d) / denom;
        t1 = (a * e - b * d) / denom;

        tvec3<T> p = l0.origin + l0.direction * t0;
        tvec3<T> q = l1.origin + l1.direction * t1;

        distance = glm::distance(p, q);
    }
    T t0;
    T t1;
    T distance;
};

} // namespace Lattice
