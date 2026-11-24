#include "GmodNodeResolution.h"

#include "viscore/utils/StringUtils.h"

#include <dnv/vista/sdk/core/Gmod.h>
#include <dnv/vista/sdk/core/GmodNode.h>
#include <dnv/vista/sdk/core/GmodPath.h>

#include <algorithm>
#include <compare>

namespace viscore::gmod
{
    const dnv::vista::sdk::GmodNode* effectiveNode(const dnv::vista::sdk::GmodNode& node)
    {
        if (const auto productType{ node.productType() }; productType.has_value())
        {
            return *productType;
        }
        if (const auto productSelection{ node.productSelection() }; productSelection.has_value())
        {
            return *productSelection;
        }

        return &node;
    }

    std::vector<const dnv::vista::sdk::GmodNode*> sortedChildren(const dnv::vista::sdk::GmodNode& node)
    {
        const auto* resolved{ effectiveNode(node) };

        std::vector<const dnv::vista::sdk::GmodNode*> children(
            resolved->children().begin(), resolved->children().end());

        std::stable_sort(
            children.begin(),
            children.end(),
            [](const dnv::vista::sdk::GmodNode* a, const dnv::vista::sdk::GmodNode* b) {
                return utils::naturalCompare(a->code(), b->code()) == std::strong_ordering::less;
            });

        return children;
    }

    std::optional<std::vector<const dnv::vista::sdk::GmodNode*>> resolveDisplayedPath(
        const dnv::vista::sdk::Gmod& gmod, const dnv::vista::sdk::GmodPath& path)
    {
        std::vector<const dnv::vista::sdk::GmodNode*> displayed{ &gmod.rootNode() };

        for (std::size_t i{ 1 }; i < path.length(); ++i)
        {
            const auto targetNodeOpt{ gmod.node(path[i].code()) };
            if (!targetNodeOpt.has_value())
            {
                return std::nullopt;
            }
            const auto* targetNode{ *targetNodeOpt };

            const auto* parentNode{ displayed.back() };
            const auto* effectiveParent{ effectiveNode(*parentNode) };
            if (effectiveParent == targetNode)
            {
                continue;
            }

            const auto children{ sortedChildren(*parentNode) };
            if (std::find(children.begin(), children.end(), targetNode) == children.end())
            {
                return std::nullopt;
            }

            displayed.push_back(targetNode);
        }

        return displayed;
    }
} // namespace viscore::gmod
