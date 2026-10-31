#pragma once

#include <string_view>

namespace dnv::vista::sdk
{
    class GmodNode;
} // namespace dnv::vista::sdk

namespace viscore::gmod
{
    enum class NodeCategory
    {
        Root = 0,
        AssetFunctionLeaf,
        OtherLeaf,
        ProductType,
        FunctionComposition,
        AssetFunctionGroup,
        ProductFunctionGroup,
        FunctionSelection,
        ProductSelection,
        Other
    };

    [[nodiscard]] NodeCategory categoryOf(const dnv::vista::sdk::GmodNode& node);
    [[nodiscard]] std::string_view categoryName(NodeCategory category);
} // namespace viscore::gmod
