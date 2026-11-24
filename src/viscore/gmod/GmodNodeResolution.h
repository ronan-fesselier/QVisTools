#pragma once

#include <optional>
#include <vector>

namespace dnv::vista::sdk
{
    class Gmod;
    class GmodNode;
    class GmodPath;
} // namespace dnv::vista::sdk

namespace viscore::gmod
{
    [[nodiscard]] const dnv::vista::sdk::GmodNode* effectiveNode(const dnv::vista::sdk::GmodNode& node);
    [[nodiscard]] std::vector<const dnv::vista::sdk::GmodNode*> sortedChildren(const dnv::vista::sdk::GmodNode& node);

    [[nodiscard]] std::optional<std::vector<const dnv::vista::sdk::GmodNode*>> resolveDisplayedPath(
        const dnv::vista::sdk::Gmod& gmod, const dnv::vista::sdk::GmodPath& path);
} // namespace viscore::gmod
