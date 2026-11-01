#include "core/vis/gmod/AncestorPath.h"

#include <dnv/vista/sdk/core/VIS.h>

#include <doctest/doctest.h>

TEST_SUITE("AncestorPath")
{
    TEST_CASE("ancestorPath starts at the root and ends at the node")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto& node{ gmod["C101"] };

        const auto path{ qvistools::core::vis::ancestorPath(node) };

        REQUIRE_FALSE(path.empty());
        CHECK(path.front() == &gmod.rootNode());
        CHECK(path.back() == &node);
    }

    TEST_CASE("ancestorPath for the root node is just the root")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto path{ qvistools::core::vis::ancestorPath(gmod.rootNode()) };

        REQUIRE(path.size() == 1);
        CHECK(path.front() == &gmod.rootNode());
    }

    TEST_CASE("ancestorPath is consistent: each node is the first parent of the next")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto& node{ gmod["C101.31"] };

        const auto path{ qvistools::core::vis::ancestorPath(node) };

        REQUIRE(path.size() > 1);
        for (std::size_t i{ 1 }; i < path.size(); ++i)
        {
            REQUIRE_FALSE(path[i]->parents().isEmpty());
            CHECK(path[i]->parents()[0] == path[i - 1]);
        }
    }

    TEST_CASE("shortAncestorPath keeps only leaf nodes, including the node itself if it is a leaf")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto& node{ gmod["C101.21"] };
        REQUIRE(node.isLeafNode());

        const auto path{ qvistools::core::vis::shortAncestorPath(node) };

        REQUIRE_FALSE(path.empty());
        CHECK(path.back() == &node);
        for (const auto* ancestor : path)
        {
            CHECK(ancestor->isLeafNode());
        }
    }

    TEST_CASE("shortAncestorPath excludes the node itself when it is not a leaf")
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        const auto& node{ gmod["C101"] };
        REQUIRE_FALSE(node.isLeafNode());

        const auto path{ qvistools::core::vis::shortAncestorPath(node) };

        for (const auto* ancestor : path)
        {
            CHECK(ancestor != &node);
        }
    }
}
