#include <catch2/catch.hpp>
#include <meshlib/Mesh.hpp>

using namespace Lattice;
using namespace glm;

TEST_CASE("Mesh::Mesh") {
    Mesh::Mesh mesh;
    SECTION("addVertex") {
        SECTION("adds verted at specified position") {
            vec3 p(1, 2, 3);
            mesh.addVertex(p);

            auto vertices = mesh.vertices() | ranges::to_vector;

            REQUIRE(vertices.size() == 1);
            REQUIRE(mesh.position(vertices[0]) == p);
        }
    }
    SECTION("addEdge") {
        vec3 p1(1, 2, 3);
        vec3 p2(4, 5, 6);

        auto v1 = mesh.addVertex(p1);
        auto v2 = mesh.addVertex(p2);
        auto edge = mesh.addEdge({v1, v2});

        SECTION("adds edge between two vertices") {
            auto edges = mesh.edges() | ranges::to_vector;
            REQUIRE(edges.size() == 1);
            REQUIRE(edges[0] == edge);

            auto vertices = mesh.vertices(edges[0]);
            REQUIRE(vertices[0] == v1);
            REQUIRE(vertices[1] == v2);
        }
    }
}
