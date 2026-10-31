#include "core/utils/StringUtils.h"
#include "core/vis/gmod/SearchIndex.h"

#include <dnv/vista/sdk/core/VIS.h>

#include <algorithm>
#include <optional>
#include <string>

#include <doctest/doctest.h>

namespace
{
    bool containsNode(
        const std::vector<qvistools::core::vis::SearchIndex::Match>& matches, const dnv::vista::sdk::GmodNode* node)
    {
        return std::any_of(matches.begin(), matches.end(), [node](const auto& match) { return match.node == node; });
    }

    // Finds the first node whose name/commonName/definition contains every word in
    // requiredWords as a whole stemmed word (matching how SearchIndex itself tokenizes
    // and stems text), not merely as a substring - "pump" must not match "pumping",
    // since search("pumps") stems to "pump" and would never match that node
    const dnv::vista::sdk::GmodNode* findFirstNodeWithWords(
        const dnv::vista::sdk::Gmod& gmod, const std::vector<std::string>& requiredWords)
    {
        for (const auto& [code, node] : gmod)
        {
            const auto& metadata{ node.metadata() };
            std::string haystack{ metadata.name() };
            if (metadata.commonName().has_value())
            {
                haystack += ' ';
                haystack += *metadata.commonName();
            }
            if (metadata.definition().has_value())
            {
                haystack += ' ';
                haystack += *metadata.definition();
            }
            haystack = qvistools::core::utils::toLower(haystack);

            const auto haystackWords{ qvistools::core::utils::splitWords(haystack) };

            const bool hasAllWords{ std::all_of(requiredWords.begin(), requiredWords.end(), [&](const auto& word) {
                const auto stemmedWord{ qvistools::core::utils::stemSimple(word) };
                return std::any_of(haystackWords.begin(), haystackWords.end(), [&](const auto& haystackWord) {
                    return qvistools::core::utils::stemSimple(haystackWord) == stemmedWord;
                });
            }) };

            if (hasAllWords)
            {
                return &node;
            }
        }
        return nullptr;
    }

    // Finds the first node reached through a product selection (i.e. a node whose
    // parent is itself skipped because it's a product selection of a FUNCTION
    // grandparent), returning {node, grandParent, parent}
    struct ProductSelectionChain
    {
        const dnv::vista::sdk::GmodNode* node;
        const dnv::vista::sdk::GmodNode* grandParent;
        const dnv::vista::sdk::GmodNode* parent;
    };

    std::optional<ProductSelectionChain> findFirstProductSelectionChain(const dnv::vista::sdk::Gmod& gmod)
    {
        for (const auto& [code, node] : gmod)
        {
            for (const auto* parent : node.parents())
            {
                for (const auto* grandParent : parent->parents())
                {
                    if (dnv::vista::sdk::Gmod::isProductSelectionAssignment(grandParent, parent))
                    {
                        return ProductSelectionChain{ &node, grandParent, parent };
                    }
                }
            }
        }
        return std::nullopt;
    }
} // namespace

TEST_SUITE("SearchIndex")
{
    TEST_CASE("search matches by node code, case-insensitively")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const qvistools::core::vis::SearchIndex index{ gmod };

        const auto& node{ gmod["C101"] };

        CHECK(containsNode(index.search("c101"), &node));
        CHECK(containsNode(index.search("C101"), &node));
    }

    TEST_CASE("search matches by substring of the node code")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const qvistools::core::vis::SearchIndex index{ gmod };

        const auto& node{ gmod["C101.31"] };

        CHECK(containsNode(index.search("C101"), &node));
    }

    TEST_CASE("search matches by node name")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const qvistools::core::vis::SearchIndex index{ gmod };

        const auto& node{ gmod.rootNode() };

        CHECK(containsNode(index.search("vessel"), &node));
    }

    TEST_CASE("search returns no match for an unknown term")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const qvistools::core::vis::SearchIndex index{ gmod };

        CHECK(index.search("zzz-not-a-real-term-zzz").empty());
    }

    TEST_CASE("search returns no match for an empty term")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const qvistools::core::vis::SearchIndex index{ gmod };

        CHECK(index.search("").empty());
    }

    TEST_CASE("search reports the root node with no parent")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const qvistools::core::vis::SearchIndex index{ gmod };

        const auto& node{ gmod.rootNode() };

        const auto matches{ index.search("vessel") };
        const auto it{ std::find_if(
            matches.begin(), matches.end(), [&node](const auto& match) { return match.node == &node; }) };

        REQUIRE(it != matches.end());
        CHECK(it->parent == nullptr);
        CHECK(it->skippedParent == nullptr);
    }

    TEST_CASE("search reports the skipped parent for a node reached through a product selection")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto chain{ findFirstProductSelectionChain(gmod) };
        REQUIRE(chain.has_value());

        const qvistools::core::vis::SearchIndex index{ gmod };

        const auto matches{ index.search(std::string{ chain->node->code() }) };
        const auto it{ std::find_if(matches.begin(), matches.end(), [&](const auto& match) {
            return match.node == chain->node && match.parent == chain->grandParent &&
                   match.skippedParent == chain->parent;
        }) };

        REQUIRE(it != matches.end());
    }

    TEST_CASE("search matches a singular field through a plural search term via stemming")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto* node{ findFirstNodeWithWords(gmod, { "pump" }) };
        REQUIRE(node != nullptr);

        const qvistools::core::vis::SearchIndex index{ gmod };

        CHECK(containsNode(index.search("pumps"), node));
    }

    TEST_CASE("search requires every stemmed word of a multi-word term to match")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto* node{ findFirstNodeWithWords(gmod, { "cooling", "pump" }) };
        REQUIRE(node != nullptr);

        const qvistools::core::vis::SearchIndex index{ gmod };

        CHECK(containsNode(index.search("cooling pump"), node));
    }

    TEST_CASE("search does not match a multi-word term when only some words are present")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto* node{ findFirstNodeWithWords(gmod, { "cooling", "pump" }) };
        REQUIRE(node != nullptr);

        const qvistools::core::vis::SearchIndex index{ gmod };

        CHECK_FALSE(containsNode(index.search("cooling zzzznotarealword"), node));
    }

    TEST_CASE("search falls back to a full scan for terms shorter than a trigram")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const qvistools::core::vis::SearchIndex index{ gmod };

        const auto& node{ gmod.rootNode() };
        REQUIRE(node.code() == "VE");

        CHECK(containsNode(index.search("VE"), &node));
    }
}
