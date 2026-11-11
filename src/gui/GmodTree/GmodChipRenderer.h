#pragma once

#include <QColor>
#include <QFontMetrics>
#include <QPoint>
#include <QString>

#include <optional>
#include <vector>

class QPainter;

namespace dnv::vista::sdk
{
    class GmodNode;
} // namespace dnv::vista::sdk

namespace qvistools::gui::chip
{
    constexpr int Height{ 18 };
    constexpr int VerticalMargin{ 4 };
    constexpr int GroupSpacing{ 6 };

    enum class Category
    {
        Root,
        LeafAssetFunction,
        Leaf,
        ProductType,
        FunctionComposition,
        AssetFunctionGroup,
        ProductFunctionGroup,
        FunctionSelection,
        ProductSelection,
        Unhandled
    };

    struct Segment
    {
        QString text;
        QColor color;
        Category category;
    };

    [[nodiscard]] Category categoryOf(const dnv::vista::sdk::GmodNode& node);
    [[nodiscard]] QColor categoryColor(Category category);
    [[nodiscard]] QString categoryName(Category category);

    [[nodiscard]] int segmentWidth(const QFontMetrics& metrics, const Segment& segment);

    int drawChipGroup(QPainter* painter, QPoint topLeft, const std::vector<Segment>& segments);

    [[nodiscard]] std::optional<std::size_t> hitTest(
        const QFontMetrics& metrics, QPoint topLeft, const std::vector<Segment>& segments, QPoint point);
} // namespace qvistools::gui::chip
