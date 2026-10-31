#pragma once

#include <vector>

namespace dnv::vista::sdk
{
    class GmodNode;
} // namespace dnv::vista::sdk

namespace qvistools::core::vis
{
    /**
     * @brief Resolve the node whose children should be displayed in place of this node's
     * @details Returns the node's product type or product selection child when applicable
     *          (transparent substitution), or the node itself otherwise
     */
    [[nodiscard]] const dnv::vista::sdk::GmodNode* effectiveNode(const dnv::vista::sdk::GmodNode& node);

    /**
     * @brief Resolve the sorted, display-ready children of a Gmod node
     * @details Substitutes a node with its product type or product selection
     *          child when applicable (transparent substitution), then sorts
     *          the resulting children by natural code order
     */
    [[nodiscard]] std::vector<const dnv::vista::sdk::GmodNode*> sortedChildren(const dnv::vista::sdk::GmodNode& node);
} // namespace qvistools::core::vis
