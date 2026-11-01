#pragma once

#include <vector>

namespace dnv::vista::sdk
{
    class GmodNode;
} // namespace dnv::vista::sdk

namespace qvistools::core::vis
{
    /**
     * @brief Resolve the ancestor path from the Gmod root down to a node, inclusive
     * @details Follows the first-parent heuristic (parents()[0]) at each step
     */
    [[nodiscard]] std::vector<const dnv::vista::sdk::GmodNode*> ancestorPath(const dnv::vista::sdk::GmodNode& node);

    /**
     * @brief Resolve the ancestor path, keeping only leaf nodes (including the node
     *        itself if it is a leaf)
     * @details Used to render a node's displayed chip path (e.g. search results):
     *          non-leaf ancestors are intermediate structure, not individually
     *          navigable/displayed segments
     */
    [[nodiscard]] std::vector<const dnv::vista::sdk::GmodNode*> shortAncestorPath(
        const dnv::vista::sdk::GmodNode& node);
} // namespace qvistools::core::vis
