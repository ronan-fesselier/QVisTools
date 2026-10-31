#pragma once

#include <vector>

namespace dnv::vista::sdk
{
    class GmodNode;
} // namespace dnv::vista::sdk

namespace viscore::gmod
{
    [[nodiscard]] const dnv::vista::sdk::GmodNode* effectiveNode(const dnv::vista::sdk::GmodNode& node);
    [[nodiscard]] std::vector<const dnv::vista::sdk::GmodNode*> sortedChildren(const dnv::vista::sdk::GmodNode& node);
} // namespace viscore::gmod
