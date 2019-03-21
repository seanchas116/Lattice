#include <catch2/catch.hpp>
#include "Mesh.hpp"

using namespace Lattice;

TEST_CASE("Mesh") {
    auto mesh = makeShared<Mesh::Mesh>();
    SECTION("addVertex") {
        glm::vec3 p(1, 2, 3);
        mesh->addVertex(p);

        REQUIRE(mesh->vertices().size() == 0);
        REQUIRE((*mesh->vertices().begin())->position() == p);
    }
}
