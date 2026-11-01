#pragma once

#include <vector>

namespace dnv::vista::sdk
{
    class GmodNode;
} // namespace dnv::vista::sdk

namespace viscore::gmod
{
    [[nodiscard]] std::vector<const dnv::vista::sdk::GmodNode*> ancestorPath(const dnv::vista::sdk::GmodNode& node);
    [[nodiscard]] std::vector<const dnv::vista::sdk::GmodNode*> shortAncestorPath(
        const dnv::vista::sdk::GmodNode& node);
} // namespace viscore::gmod
