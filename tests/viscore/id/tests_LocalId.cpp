#include <viscore/id/LocalId.h>

#include <dnv/vista/sdk/core/VIS.h>

#include <doctest/doctest.h>

TEST_SUITE("LocalId")
{
    TEST_CASE("buildLocalId builds a valid LocalId from a primary item and one metadata tag")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };
        const auto& locations{ vis.locations(vis.latest()) };

        const auto primaryItem{ dnv::vista::sdk::GmodPath::fromShortPath("411.1/C101.31-2", gmod, locations) };
        REQUIRE(primaryItem.has_value());

        const auto result{ viscore::id::buildLocalId(
            vis.latest(),
            primaryItem,
            std::nullopt,
            { { dnv::vista::sdk::CodebookName::Quantity, "temperature" } },
            false) };

        REQUIRE(result.localId.has_value());
        CHECK(result.errors.empty());
        CHECK(*result.localId == "/dnv-v2/vis-3-11a/411.1/C101.31-2/meta/qty-temperature");
    }

    TEST_CASE("buildLocalId reports an error for a missing primary item")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };

        const auto result{ viscore::id::buildLocalId(
            vis.latest(),
            std::nullopt,
            std::nullopt,
            { { dnv::vista::sdk::CodebookName::Quantity, "temperature" } },
            false) };

        CHECK_FALSE(result.localId.has_value());
        REQUIRE_FALSE(result.errors.empty());
    }

    TEST_CASE("buildLocalId reports an error for an invalid metadata tag value")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };
        const auto& locations{ vis.locations(vis.latest()) };

        const auto primaryItem{ dnv::vista::sdk::GmodPath::fromShortPath("411.1/C101.31-2", gmod, locations) };
        REQUIRE(primaryItem.has_value());

        const auto result{ viscore::id::buildLocalId(
            vis.latest(),
            primaryItem,
            std::nullopt,
            { { dnv::vista::sdk::CodebookName::Quantity, "!not-a-valid-quantity-value" } },
            false) };

        CHECK_FALSE(result.localId.has_value());
        REQUIRE_FALSE(result.errors.empty());
        CHECK(result.errors.front().type == viscore::id::LocalIdBuildErrorType::InvalidTag);
    }
}
