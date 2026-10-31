#pragma once

#include <vector>

namespace dnv::vista::sdk
{
    class GmodNode;
} // namespace dnv::vista::sdk

namespace qvistools::core::vis
{
    /**
     * @brief Resolve the sorted, display-ready children of a Gmod node
     * @details Substitutes a node with its product type or product selection
     *          child when applicable (transparent substitution), then sorts
     *          the resulting children by natural code order
     */
    [[nodiscard]] std::vector<const dnv::vista::sdk::GmodNode*> sortedChildren(const dnv::vista::sdk::GmodNode& node);
} // namespace qvistools::core::vis
