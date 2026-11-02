#include <viscore/codebooks/CodebookValues.h>

#include <dnv/vista/sdk/core/VIS.h>

#include <algorithm>
#include <set>

#include <doctest/doctest.h>

TEST_SUITE("Codebooks")
{
    TEST_CASE("DISPLAYED_CODEBOOKS has no duplicate entries")
    {
        const std::set<dnv::vista::sdk::CodebookName> unique{ viscore::codebooks::DISPLAYED_CODEBOOKS.begin(),
                                                              viscore::codebooks::DISPLAYED_CODEBOOKS.end() };
        CHECK(unique.size() == viscore::codebooks::DISPLAYED_CODEBOOKS.size());
    }

    TEST_CASE("DISPLAYED_CODEBOOKS includes the Quantity and Position codebooks")
    {
        const auto contains{ [](dnv::vista::sdk::CodebookName name) {
            return std::find(
                       viscore::codebooks::DISPLAYED_CODEBOOKS.begin(),
                       viscore::codebooks::DISPLAYED_CODEBOOKS.end(),
                       name) != viscore::codebooks::DISPLAYED_CODEBOOKS.end();
        } };

        CHECK(contains(dnv::vista::sdk::CodebookName::Quantity));
        CHECK(contains(dnv::vista::sdk::CodebookName::Position));
    }

    TEST_CASE("standardValuesOf resolves every displayed codebook without throwing")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& codebooks{ vis.codebooks(vis.latest()) };

        for (const auto name : viscore::codebooks::DISPLAYED_CODEBOOKS)
        {
            std::vector<std::string> values;
            CHECK_NOTHROW(values = viscore::codebooks::standardValuesOf(codebooks, name));
        }
    }

    TEST_CASE("standardValuesOf returns a sorted, non-empty list of standard values for Quantity")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& codebooks{ vis.codebooks(vis.latest()) };

        const auto values{ viscore::codebooks::standardValuesOf(codebooks, dnv::vista::sdk::CodebookName::Quantity) };

        REQUIRE_FALSE(values.empty());
        CHECK(std::is_sorted(values.begin(), values.end()));

        CHECK(std::find(values.begin(), values.end(), "temperature") != values.end());
    }
}
