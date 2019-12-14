#pragma once

#include <array>
#include <glm/glm.hpp>
#include <tuple>

namespace Lattice {

template <typename T>
class Ray final {
  public:
    Ray() = default;
    Ray(glm::tvec3<T> origin, glm::tvec3<T> direction) : origin(origin), direction(direction) {}

    template <typename U>
    Ray(const Ray<U> &other) : origin(other.origin), direction(other.direction) {}

    template <typename U>
    static Ray fromPoints(const std::array<glm::tvec3<U>, 2> &points) {
        return Ray(points[0], points[1] - points[0]);
    }

    std::tuple<bool, float> intersectsTriangle(const std::array<glm::tvec3<T>, 3> &triangle) const {
        // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
        using namespace glm;
        const T EPSILON = 0.0000001;
        auto [vertex0, vertex1, vertex2] = triangle;
        tvec3<T> edge1, edge2, h, s, q;
        T a, f, u, v;
        edge1 = vertex1 - vertex0;
        edge2 = vertex2 - vertex0;
        h = cross(direction, edge2);
        a = dot(edge1, h);
        if (a > -EPSILON && a < EPSILON) {
            return {false, 0}; // This ray is parallel to this triangle.
        }
        f = 1 / a;
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

    glm::tvec3<T> at(T t) const {
        return origin + direction * t;
    }

    glm::tvec3<T> whereXIsZero() const { // TODO: find better name
        glm::tvec3<T> p0 = origin;
        glm::tvec3<T> p1 = direction + origin;

        glm::tvec2<T> yz = (p1.x * p0.yz - p0.x * p1.yz) / (p1.x - p0.x);
        return {0, yz};
    }

    glm::tvec3<T> origin{0};
    glm::tvec3<T> direction{0};
};

template <typename T>
Ray<T> operator*(const glm::tmat4x4<T> &mat, const Ray<T> &ray) {
    return {
        (mat * glm::dvec4(ray.origin, 1)).xyz,
        (mat * glm::dvec4(ray.direction, 0)).xyz,
    };
}

} // namespace Lattice
