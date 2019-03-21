#include <catch2/catch.hpp>
#include "Mesh.hpp"

using namespace Lattice;

TEST_CASE("Mesh") {
    SECTION("test") {
        auto mesh = makeShared<Mesh::Mesh>();
        REQUIRE(mesh->vertices().size() == 0);
    }
}
