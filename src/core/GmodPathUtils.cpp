#include "GmodPathUtils.h"

#include <dnv/vista/sdk/core/GmodNode.h>

#include <algorithm>

namespace qvistools::core
{
    std::vector<const dnv::vista::sdk::GmodNode*> ancestorPath(const dnv::vista::sdk::GmodNode& fromParent)
    {
        std::vector<const dnv::vista::sdk::GmodNode*> path;

        const dnv::vista::sdk::GmodNode* current{ &fromParent };
        path.push_back(current);
        while (!current->parents().isEmpty())
        {
            const dnv::vista::sdk::GmodNode* parent{ current->parents()[0] };
            path.push_back(parent);
            current = parent;
        }

        std::reverse(path.begin(), path.end());
        return path;
    }

    std::vector<const dnv::vista::sdk::GmodNode*> shortAncestorPath(const dnv::vista::sdk::GmodNode& fromParent)
    {
        std::vector<const dnv::vista::sdk::GmodNode*> path;

        for (const auto* ancestor : ancestorPath(fromParent))
        {
            if (ancestor->isLeafNode())
            {
                path.push_back(ancestor);
            }
        }

        return path;
    }
} // namespace qvistools::core
