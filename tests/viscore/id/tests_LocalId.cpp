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

TEST_SUITE("convertLocalId")
{
    TEST_CASE("convertLocalId converts a LocalIdBuilder from an older VIS version to the latest")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };

        constexpr auto sourceVersion{ dnv::vista::sdk::VisVersion::v3_4a };
        const auto& sourceGmod{ vis.gmod(sourceVersion) };
        const auto& sourceLocations{ vis.locations(sourceVersion) };

        const auto primaryItem{ dnv::vista::sdk::GmodPath::fromShortPath(
            "411.1/C101.31-2", sourceGmod, sourceLocations) };
        REQUIRE(primaryItem.has_value());

        const auto builderResult{ viscore::id::buildLocalIdBuilder(
            sourceVersion,
            primaryItem,
            std::nullopt,
            { { dnv::vista::sdk::CodebookName::Quantity, "temperature" } },
            false) };
        REQUIRE(builderResult.builder.has_value());

        const auto result{ viscore::id::convertLocalId(*builderResult.builder, vis.latest()) };

        REQUIRE(result.builder.has_value());
        CHECK(result.errors.empty());
        CHECK(result.builder->version() == vis.latest());
    }

    TEST_CASE("convertLocalId reports an error when the target version is not strictly greater than the source")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };
        const auto& locations{ vis.locations(vis.latest()) };

        const auto primaryItem{ dnv::vista::sdk::GmodPath::fromShortPath("411.1/C101.31-2", gmod, locations) };
        REQUIRE(primaryItem.has_value());

        const auto builderResult{ viscore::id::buildLocalIdBuilder(
            vis.latest(),
            primaryItem,
            std::nullopt,
            { { dnv::vista::sdk::CodebookName::Quantity, "temperature" } },
            false) };
        REQUIRE(builderResult.builder.has_value());

        const auto result{ viscore::id::convertLocalId(*builderResult.builder, vis.latest()) };

        CHECK_FALSE(result.builder.has_value());
        REQUIRE_FALSE(result.errors.empty());
        CHECK(result.errors.front().type == viscore::id::LocalIdConvertErrorType::ValidationError);
    }
}
