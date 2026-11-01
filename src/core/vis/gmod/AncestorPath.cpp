#include "AncestorPath.h"

#include <dnv/vista/sdk/core/GmodNode.h>

#include <algorithm>

namespace qvistools::core::vis
{
    std::vector<const dnv::vista::sdk::GmodNode*> ancestorPath(const dnv::vista::sdk::GmodNode& node)
    {
        std::vector<const dnv::vista::sdk::GmodNode*> path;

        const dnv::vista::sdk::GmodNode* current{ &node };
        path.push_back(current);
        while (!current->parents().isEmpty())
        {
            current = current->parents()[0];
            path.push_back(current);
        }

        std::reverse(path.begin(), path.end());
        return path;
    }

    std::vector<const dnv::vista::sdk::GmodNode*> shortAncestorPath(const dnv::vista::sdk::GmodNode& node)
    {
        std::vector<const dnv::vista::sdk::GmodNode*> path;

        for (const auto* ancestor : ancestorPath(node))
        {
            if (ancestor->isLeafNode())
            {
                path.push_back(ancestor);
            }
        }

        return path;
    }
} // namespace qvistools::core::vis
