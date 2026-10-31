#include <viscore/gmod/NodeCategory.h>

#include <dnv/vista/sdk/core/VIS.h>

#include <string_view>
#include <vector>

#include <doctest/doctest.h>

namespace
{
    template <typename Predicate>
    const dnv::vista::sdk::GmodNode* findFirstNode(const dnv::vista::sdk::Gmod& gmod, Predicate predicate)
    {
        for (const auto& [code, node] : gmod)
        {
            if (predicate(node))
            {
                return &node;
            }
        }
        return nullptr;
    }
} // namespace

TEST_SUITE("NodeCategory")
{
    TEST_CASE("categoryOf returns Root for the Gmod root node")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        CHECK(viscore::gmod::categoryOf(gmod.rootNode()) == viscore::gmod::NodeCategory::Root);
    }

    TEST_CASE("categoryOf returns AssetFunctionGroup for an asset function GROUP node")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto* node{ findFirstNode(gmod, [](const auto& n) {
            return n.metadata().type() == "GROUP" && n.metadata().category() == "ASSET FUNCTION";
        }) };
        REQUIRE(node != nullptr);

        CHECK(viscore::gmod::categoryOf(*node) == viscore::gmod::NodeCategory::AssetFunctionGroup);
    }

    TEST_CASE("categoryOf returns ProductFunctionGroup for a product function GROUP node")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto* node{ findFirstNode(gmod, [](const auto& n) {
            return n.metadata().type() == "GROUP" && n.metadata().category() == "PRODUCT FUNCTION";
        }) };
        REQUIRE(node != nullptr);

        CHECK(viscore::gmod::categoryOf(*node) == viscore::gmod::NodeCategory::ProductFunctionGroup);
    }

    TEST_CASE("categoryOf returns ProductSelection for a product selection node")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto* node{ findFirstNode(gmod, [](const auto& n) { return n.isProductSelection(); }) };
        REQUIRE(node != nullptr);

        CHECK(viscore::gmod::categoryOf(*node) == viscore::gmod::NodeCategory::ProductSelection);
    }

    TEST_CASE("categoryOf returns FunctionSelection for a function selection node")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto* node{ findFirstNode(gmod, [](const auto& n) {
            return n.metadata().type() == "SELECTION" &&
                   (n.metadata().category() == "ASSET FUNCTION" || n.metadata().category() == "PRODUCT FUNCTION");
        }) };
        REQUIRE(node != nullptr);

        CHECK(viscore::gmod::categoryOf(*node) == viscore::gmod::NodeCategory::FunctionSelection);
    }

    TEST_CASE("categoryOf returns ProductType for a product type node")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto* node{ findFirstNode(gmod, [](const auto& n) { return n.isProductType(); }) };
        REQUIRE(node != nullptr);

        CHECK(viscore::gmod::categoryOf(*node) == viscore::gmod::NodeCategory::ProductType);
    }

    TEST_CASE("categoryOf returns AssetFunctionLeaf for an asset function leaf node")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto* node{ findFirstNode(
            gmod, [](const auto& n) { return n.isLeafNode() && n.isAssetFunctionNode(); }) };
        REQUIRE(node != nullptr);

        CHECK(viscore::gmod::categoryOf(*node) == viscore::gmod::NodeCategory::AssetFunctionLeaf);
    }

    TEST_CASE("categoryOf returns OtherLeaf for a leaf node that is not an asset function")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto* node{ findFirstNode(
            gmod, [](const auto& n) { return n.isLeafNode() && !n.isAssetFunctionNode(); }) };
        REQUIRE(node != nullptr);

        CHECK(viscore::gmod::categoryOf(*node) == viscore::gmod::NodeCategory::OtherLeaf);
    }

    TEST_CASE("categoryOf returns FunctionComposition for a function composition node")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto* node{ findFirstNode(gmod, [](const auto& n) { return n.isFunctionComposition(); }) };
        REQUIRE(node != nullptr);

        CHECK(viscore::gmod::categoryOf(*node) == viscore::gmod::NodeCategory::FunctionComposition);
    }

    TEST_CASE("categoryName returns a non-empty, distinct name for every category")
    {
        constexpr viscore::gmod::NodeCategory categories[]{ viscore::gmod::NodeCategory::Root,
                                                            viscore::gmod::NodeCategory::AssetFunctionLeaf,
                                                            viscore::gmod::NodeCategory::OtherLeaf,
                                                            viscore::gmod::NodeCategory::ProductType,
                                                            viscore::gmod::NodeCategory::FunctionComposition,
                                                            viscore::gmod::NodeCategory::AssetFunctionGroup,
                                                            viscore::gmod::NodeCategory::ProductFunctionGroup,
                                                            viscore::gmod::NodeCategory::FunctionSelection,
                                                            viscore::gmod::NodeCategory::ProductSelection,
                                                            viscore::gmod::NodeCategory::Other };

        std::vector<std::string_view> names;
        for (const auto category : categories)
        {
            const auto name{ viscore::gmod::categoryName(category) };
            CHECK_FALSE(name.empty());
            names.emplace_back(name);
        }

        for (std::size_t i{ 0 }; i < names.size(); ++i)
        {
            for (std::size_t j{ i + 1 }; j < names.size(); ++j)
            {
                CHECK(names[i] != names[j]);
            }
        }
    }
}
