#include "core/vis/codebooks/CodebookValues.h"

#include <dnv/vista/sdk/core/VIS.h>

#include <algorithm>

#include <doctest/doctest.h>

TEST_SUITE("Codebooks")
{
    TEST_CASE("DISPLAYED_CODEBOOKS lists exactly the 8 codebooks shown by the DNV Local ID builder")
    {
        CHECK(qvistools::core::vis::DISPLAYED_CODEBOOKS.size() == 8);

        const std::array<dnv::vista::sdk::CodebookName, 8> expected{
            dnv::vista::sdk::CodebookName::Quantity,    dnv::vista::sdk::CodebookName::Content,
            dnv::vista::sdk::CodebookName::Calculation, dnv::vista::sdk::CodebookName::State,
            dnv::vista::sdk::CodebookName::Command,     dnv::vista::sdk::CodebookName::Type,
            dnv::vista::sdk::CodebookName::Position,    dnv::vista::sdk::CodebookName::Detail
        };
        CHECK(qvistools::core::vis::DISPLAYED_CODEBOOKS == expected);
    }

    TEST_CASE("standardValuesOf returns a sorted, non-empty list of standard values for Quantity")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& codebooks{ vis.codebooks(vis.latest()) };

        const auto values{ qvistools::core::vis::standardValuesOf(codebooks, dnv::vista::sdk::CodebookName::Quantity) };

        REQUIRE_FALSE(values.empty());
        CHECK(std::is_sorted(values.begin(), values.end()));

        CHECK(std::find(values.begin(), values.end(), "temperature") != values.end());
    }
}
