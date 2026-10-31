#pragma once

#include <string_view>

namespace dnv::vista::sdk
{
    class GmodNode;
} // namespace dnv::vista::sdk

namespace qvistools::core::vis
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

    /**
     * @brief Classify a Gmod node into a display category
     * @details Checks are mutually exclusive by priority: root first, then leaf
     *          (split into asset-function vs other), then product type, then
     *          function composition, then GROUP/SELECTION metadata types (further
     *          split by whether the node's own category is an asset/product function
     *          or a plain product), with Other as the fallback for any unhandled
     *          node shape
     */
    [[nodiscard]] NodeCategory categoryOf(const dnv::vista::sdk::GmodNode& node);

    /**
     * @brief Human-readable name for a node category, for display (e.g. tooltips)
     */
    [[nodiscard]] std::string_view categoryName(NodeCategory category);
} // namespace qvistools::core::vis
