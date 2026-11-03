#include <viscore/id/LocalId.h>
#include <viscore/id/UniversalId.h>

#include <dnv/vista/sdk/core/VIS.h>

#include <doctest/doctest.h>

TEST_SUITE("UniversalId")
{
    TEST_CASE("buildUniversalId builds a valid UniversalId from a valid IMO number and LocalIdBuilder")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };
        const auto& locations{ vis.locations(vis.latest()) };

        const auto primaryItem{ dnv::vista::sdk::GmodPath::fromShortPath("411.1/C101.31-2", gmod, locations) };
        REQUIRE(primaryItem.has_value());

        auto builderResult{ viscore::id::buildLocalIdBuilder(
            vis.latest(),
            primaryItem,
            std::nullopt,
            { { dnv::vista::sdk::CodebookName::Quantity, "temperature" } },
            false) };
        REQUIRE(builderResult.builder.has_value());

        const auto result{ viscore::id::buildUniversalId(vis.latest(), "1234567", *builderResult.builder) };

        REQUIRE(result.universalId.has_value());
        CHECK(result.errors.empty());
        CHECK(*result.universalId == "data.dnv.com/IMO1234567/dnv-v2/vis-3-11a/411.1/C101.31-2/meta/qty-temperature");
    }

    TEST_CASE("buildUniversalId reports an error for an invalid IMO number")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };
        const auto& locations{ vis.locations(vis.latest()) };

        const auto primaryItem{ dnv::vista::sdk::GmodPath::fromShortPath("411.1/C101.31-2", gmod, locations) };
        REQUIRE(primaryItem.has_value());

        auto builderResult{ viscore::id::buildLocalIdBuilder(vis.latest(), primaryItem, std::nullopt, {}, false) };
        REQUIRE(builderResult.builder.has_value());

        const auto result{ viscore::id::buildUniversalId(vis.latest(), "not-an-imo", *builderResult.builder) };

        CHECK_FALSE(result.universalId.has_value());
        REQUIRE_FALSE(result.errors.empty());
        CHECK(result.errors.front().type == viscore::id::UniversalIdBuildErrorType::InvalidImoNumber);
    }
}
