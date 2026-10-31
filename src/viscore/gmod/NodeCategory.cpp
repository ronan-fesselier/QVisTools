#include "NodeCategory.h"

#include <dnv/vista/sdk/core/GmodNode.h>

namespace viscore::gmod
{
    NodeCategory categoryOf(const dnv::vista::sdk::GmodNode& node)
    {
        if (node.isRoot())
        {
            return NodeCategory::Root;
        }
        if (node.isLeafNode())
        {
            return node.isAssetFunctionNode() ? NodeCategory::AssetFunctionLeaf : NodeCategory::OtherLeaf;
        }
        if (node.isProductType())
        {
            return NodeCategory::ProductType;
        }
        if (node.isFunctionComposition())
        {
            return NodeCategory::FunctionComposition;
        }

        if (node.metadata().type() == "GROUP")
        {
            const auto& category{ node.metadata().category() };
            if (category == "ASSET FUNCTION")
            {
                return NodeCategory::AssetFunctionGroup;
            }
            if (category == "PRODUCT FUNCTION")
            {
                return NodeCategory::ProductFunctionGroup;
            }
            return NodeCategory::Other;
        }
        if (node.metadata().type() == "SELECTION")
        {
            const auto& category{ node.metadata().category() };
            if (category == "ASSET FUNCTION" || category == "PRODUCT FUNCTION")
            {
                return NodeCategory::FunctionSelection;
            }
            if (node.isProductSelection())
            {
                return NodeCategory::ProductSelection;
            }
            return NodeCategory::Other;
        }

        return NodeCategory::Other;
    }

    std::string_view categoryName(NodeCategory category)
    {
        switch (category)
        {
            case NodeCategory::Root:
                return "Root";
            case NodeCategory::AssetFunctionLeaf:
                return "Leaf - asset function";
            case NodeCategory::OtherLeaf:
                return "Leaf";
            case NodeCategory::ProductType:
                return "Product type";
            case NodeCategory::FunctionComposition:
                return "Function composition";
            case NodeCategory::AssetFunctionGroup:
                return "Asset function group";
            case NodeCategory::ProductFunctionGroup:
                return "Product function group";
            case NodeCategory::FunctionSelection:
                return "Function selection";
            case NodeCategory::ProductSelection:
                return "Product selection";
            case NodeCategory::Other:
                break;
        }

        return "Unhandled category";
    }
} // namespace viscore::gmod
