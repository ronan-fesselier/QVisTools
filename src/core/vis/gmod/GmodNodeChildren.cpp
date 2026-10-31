#include "GmodNodeChildren.h"

#include "core/utils/StringUtils.h"

#include <dnv/vista/sdk/core/GmodNode.h>

#include <algorithm>
#include <compare>

namespace qvistools::core::vis
{
    std::vector<const dnv::vista::sdk::GmodNode*> sortedChildren(const dnv::vista::sdk::GmodNode& node)
    {
        const auto* effectiveNode{ &node };
        if (const auto productType{ node.productType() }; productType.has_value())
        {
            effectiveNode = *productType;
        }
        else if (const auto productSelection{ node.productSelection() }; productSelection.has_value())
        {
            effectiveNode = *productSelection;
        }

        std::vector<const dnv::vista::sdk::GmodNode*> children(
            effectiveNode->children().begin(), effectiveNode->children().end());

        std::stable_sort(
            children.begin(),
            children.end(),
            [](const dnv::vista::sdk::GmodNode* a, const dnv::vista::sdk::GmodNode* b) {
                return utils::naturalCompare(a->code(), b->code()) == std::strong_ordering::less;
            });

        return children;
    }
} // namespace qvistools::core::vis
