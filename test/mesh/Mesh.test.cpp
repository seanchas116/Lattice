#include <catch2/catch.hpp>
#include "../../src/mesh/Mesh.hpp"

using namespace Lattice;
using namespace glm;

TEST_CASE("Mesh") {
    auto mesh = makeShared<OldMesh::Mesh>();
    SECTION("addVertex") {
        SECTION("adds verted at specified position") {
            dvec3 p(1, 2, 3);
            mesh->addVertex(p);

            REQUIRE(mesh->vertices().size() == 1);
            REQUIRE((*mesh->vertices().begin())->position() == p);
        }
    }
    SECTION("addEdge") {
        dvec3 p1(1, 2, 3);
        dvec3 p2(4, 5, 6);

        auto v1 = mesh->addVertex(p1);
        auto v2 = mesh->addVertex(p2);
        auto edge = mesh->addEdge({v1, v2});

        SECTION("adds edge between two vertices") {
            REQUIRE(mesh->edges().size() == 1);
            REQUIRE(mesh->edges().begin()->second == edge);
            REQUIRE(edge->vertices()[0] == v1);
            REQUIRE(edge->vertices()[1] == v2);
        }
    }
}
