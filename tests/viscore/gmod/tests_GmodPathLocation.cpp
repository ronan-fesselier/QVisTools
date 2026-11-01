#include <viscore/gmod/GmodPathLocation.h>

#include <dnv/vista/sdk/core/LocationBuilder.h>
#include <dnv/vista/sdk/core/VIS.h>

#include <doctest/doctest.h>

TEST_SUITE("GmodPathLocation")
{
    TEST_CASE("applyLocationToSet assigns a composite location to the target set")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };
        const auto& locations{ vis.locations(vis.latest()) };

        const auto path{ dnv::vista::sdk::GmodPath::fromShortPath("411.1/C101.62/S205", gmod, locations) };
        REQUIRE(path.has_value());
        REQUIRE(path->isIndividualizable());

        const auto location{ dnv::vista::sdk::LocationBuilder::create(locations)
                                 .withNumber(3)
                                 .withSide('P')
                                 .withVertical('U')
                                 .withLongitudinal('F')
                                 .build() };

        const auto result{ viscore::gmod::applyLocationToSet(*path, 0, location) };

        REQUIRE(result.path.has_value());
        CHECK_FALSE(result.error.has_value());
        CHECK(result.path->toString() == "411.1-3FPU/C101.62/S205");
    }

    TEST_CASE("applyLocationToSet rejects an out-of-range set index")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };
        const auto& locations{ vis.locations(vis.latest()) };

        const auto path{ dnv::vista::sdk::GmodPath::fromShortPath("411.1/C101.62/S205", gmod, locations) };
        REQUIRE(path.has_value());

        const auto location{ dnv::vista::sdk::LocationBuilder::create(locations).withNumber(1).build() };

        const auto result{ viscore::gmod::applyLocationToSet(*path, 999, location) };

        CHECK_FALSE(result.path.has_value());
        REQUIRE(result.error.has_value());
    }
}
