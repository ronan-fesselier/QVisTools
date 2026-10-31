#include "core/utils/StringUtils.h"
#include "core/vis/gmod/GmodNodeChildren.h"

#include <dnv/vista/sdk/core/VIS.h>

#include <algorithm>

#include <doctest/doctest.h>

namespace
{
    std::vector<const dnv::vista::sdk::GmodNode*> naturallySorted(const auto& children)
    {
        std::vector<const dnv::vista::sdk::GmodNode*> sorted(children.begin(), children.end());
        std::stable_sort(sorted.begin(), sorted.end(), [](const auto* a, const auto* b) {
            return qvistools::core::utils::naturalCompare(a->code(), b->code()) == std::strong_ordering::less;
        });
        return sorted;
    }
} // namespace

TEST_SUITE("Gmod")
{
    TEST_CASE("sortedChildren substitutes a node with its product type child")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto& node{ gmod["411.3"] };
        REQUIRE(node.productType().has_value());

        const auto* productType{ *node.productType() };

        const auto children{ qvistools::core::vis::sortedChildren(node) };

        CHECK(children == naturallySorted(productType->children()));
    }

    TEST_CASE("sortedChildren substitutes a node with its product selection child")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto& node{ gmod["411.2"] };
        REQUIRE(node.productSelection().has_value());

        const auto* productSelection{ *node.productSelection() };

        const auto children{ qvistools::core::vis::sortedChildren(node) };

        CHECK(children == naturallySorted(productSelection->children()));
    }

    TEST_CASE("sortedChildren sorts children by natural code order")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto& root{ gmod.rootNode() };

        const auto children{ qvistools::core::vis::sortedChildren(root) };

        REQUIRE(children.size() > 1);

        for (std::size_t i{ 1 }; i < children.size(); ++i)
        {
            CHECK(
                qvistools::core::utils::naturalCompare(children[i - 1]->code(), children[i]->code()) !=
                std::strong_ordering::greater);
        }
    }
}
