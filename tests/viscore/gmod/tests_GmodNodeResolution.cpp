#include <viscore/gmod/GmodNodeResolution.h>
#include <viscore/utils/StringUtils.h>

#include <dnv/vista/sdk/core/GmodPath.h>
#include <dnv/vista/sdk/core/VIS.h>

#include <algorithm>

#include <doctest/doctest.h>

namespace
{
    std::vector<const dnv::vista::sdk::GmodNode*> naturallySorted(const auto& children)
    {
        std::vector<const dnv::vista::sdk::GmodNode*> sorted(children.begin(), children.end());
        std::stable_sort(sorted.begin(), sorted.end(), [](const auto* a, const auto* b) {
            return viscore::utils::naturalCompare(a->code(), b->code()) == std::strong_ordering::less;
        });
        return sorted;
    }
} // namespace

TEST_SUITE("GmodNode resolution")
{
    TEST_CASE("effectiveNode resolves to the product type child when applicable")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto& node{ gmod["411.3"] };
        REQUIRE(node.productType().has_value());

        CHECK(viscore::gmod::effectiveNode(node) == *node.productType());
    }

    TEST_CASE("effectiveNode resolves to the product selection child when applicable")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto& node{ gmod["411.2"] };
        REQUIRE(node.productSelection().has_value());

        CHECK(viscore::gmod::effectiveNode(node) == *node.productSelection());
    }

    TEST_CASE("effectiveNode resolves to the node itself when no substitution applies")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto& node{ gmod.rootNode() };
        REQUIRE_FALSE(node.productType().has_value());
        REQUIRE_FALSE(node.productSelection().has_value());

        CHECK(viscore::gmod::effectiveNode(node) == &node);
    }

    TEST_CASE("sortedChildren substitutes a node with its product type child")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto& node{ gmod["411.3"] };
        REQUIRE(node.productType().has_value());

        const auto* productType{ *node.productType() };

        const auto children{ viscore::gmod::sortedChildren(node) };

        CHECK(children == naturallySorted(productType->children()));
    }

    TEST_CASE("sortedChildren substitutes a node with its product selection child")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto& node{ gmod["411.2"] };
        REQUIRE(node.productSelection().has_value());

        const auto* productSelection{ *node.productSelection() };

        const auto children{ viscore::gmod::sortedChildren(node) };

        CHECK(children == naturallySorted(productSelection->children()));
    }

    TEST_CASE("sortedChildren sorts children by natural code order")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto& root{ gmod.rootNode() };

        const auto children{ viscore::gmod::sortedChildren(root) };

        REQUIRE(children.size() > 1);

        for (std::size_t i{ 1 }; i < children.size(); ++i)
        {
            CHECK(
                viscore::utils::naturalCompare(children[i - 1]->code(), children[i]->code()) !=
                std::strong_ordering::greater);
        }
    }
}

TEST_SUITE("resolveDisplayedPath")
{
    TEST_CASE("a GmodPath always starts at the gmod root node")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };
        const auto& locations{ vis.locations(vis.latest()) };

        const auto path{ dnv::vista::sdk::GmodPath::fromShortPath("411.1/C101.31-2", gmod, locations) };
        REQUIRE(path.has_value());

        CHECK(path->parents()[0].code() == gmod.rootNode().code());
    }

    TEST_CASE("resolves a simple path to its displayed node sequence")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };
        const auto& locations{ vis.locations(vis.latest()) };

        const auto path{ dnv::vista::sdk::GmodPath::fromShortPath("411.1/C101.31-2", gmod, locations) };
        REQUIRE(path.has_value());

        const auto displayed{ viscore::gmod::resolveDisplayedPath(gmod, *path) };
        REQUIRE(displayed.has_value());

        REQUIRE_FALSE(displayed->empty());
        CHECK(displayed->front() == &gmod.rootNode());
        CHECK(displayed->back()->code() == path->node().code());
    }

    TEST_CASE("resolves a path through a skipped product selection parent")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto& node{ gmod["411.2"] };
        REQUIRE(node.productSelection().has_value());
        const auto* productSelection{ *node.productSelection() };
        REQUIRE_FALSE(productSelection->children().isEmpty());

        const auto* child{ productSelection->children()[0] };

        std::vector<dnv::vista::sdk::GmodNode> ancestors;
        for (const auto* ancestor{ node.parents()[0] }; ancestor != &gmod.rootNode(); ancestor = ancestor->parents()[0])
        {
            ancestors.push_back(*ancestor);
        }
        std::reverse(ancestors.begin(), ancestors.end());

        std::vector<dnv::vista::sdk::GmodNode> parents{ gmod.rootNode() };
        parents.insert(parents.end(), ancestors.begin(), ancestors.end());
        parents.push_back(node);
        parents.push_back(*productSelection);

        const dnv::vista::sdk::GmodPath path{ std::move(parents), *child };

        const auto displayed{ viscore::gmod::resolveDisplayedPath(gmod, path) };
        REQUIRE(displayed.has_value());
        CHECK(displayed->back() == child);
    }

    TEST_CASE("round-trips: sortedChildren navigation matches resolveDisplayedPath for the same path")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };
        const auto& locations{ vis.locations(vis.latest()) };

        const auto path{ dnv::vista::sdk::GmodPath::fromShortPath("411.1/C101.31-2", gmod, locations) };
        REQUIRE(path.has_value());

        const auto displayed{ viscore::gmod::resolveDisplayedPath(gmod, *path) };
        REQUIRE(displayed.has_value());

        for (std::size_t i{ 1 }; i < displayed->size(); ++i)
        {
            const auto* parent{ (*displayed)[i - 1] };
            const auto* child{ (*displayed)[i] };

            if (viscore::gmod::effectiveNode(*parent) == child)
            {
                continue;
            }

            const auto children{ viscore::gmod::sortedChildren(*parent) };
            CHECK(std::find(children.begin(), children.end(), child) != children.end());
        }
    }
}
