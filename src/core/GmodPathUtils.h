#pragma once

#include <string>
#include <vector>

namespace dnv::vista::sdk
{
    class GmodNode;
} // namespace dnv::vista::sdk

namespace qvistools::core
{
    [[nodiscard]] std::vector<const dnv::vista::sdk::GmodNode*> ancestorPath(
        const dnv::vista::sdk::GmodNode& fromParent);

    [[nodiscard]] std::vector<const dnv::vista::sdk::GmodNode*> shortAncestorPath(
        const dnv::vista::sdk::GmodNode& fromParent);
} // namespace qvistools::core
