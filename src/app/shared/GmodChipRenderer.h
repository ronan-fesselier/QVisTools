#pragma once

#include <viscore/gmod/NodeCategory.h>

#include <QColor>
#include <QModelIndex>
#include <QPoint>
#include <QString>

#include <optional>
#include <vector>

class QFontMetrics;
class QPainter;
class QStyleOptionViewItem;

namespace qvistools
{
    constexpr int CHIP_HEIGHT{ 18 };
    constexpr int CHIP_RADIUS{ CHIP_HEIGHT / 2 };
    constexpr int CHIP_PADDING_H{ 6 };
    constexpr int CHIP_GROUP_SPACING{ 6 };
    constexpr int CHIP_VERTICAL_MARGIN{ 4 };

    [[nodiscard]] QColor categoryColor(viscore::gmod::NodeCategory category);
    [[nodiscard]] QColor readableTextColor(const QColor& background);

    enum class ChipIcon
    {
        None,
        Link
    };

    struct ChipSegment
    {
        QString text;
        QColor color;
        viscore::gmod::NodeCategory category;
        ChipIcon icon{ ChipIcon::None };
    };

    [[nodiscard]] int segmentWidth(const QFontMetrics& metrics, const ChipSegment& segment);
    int drawChipGroup(QPainter* painter, QPoint topLeft, const std::vector<ChipSegment>& segments);
    void paintChipRow(
        QPainter* painter,
        const QStyleOptionViewItem& initializedOption,
        const QString& name,
        const std::vector<ChipSegment>& segments);

    [[nodiscard]] std::optional<std::size_t> hitTest(
        const QFontMetrics& metrics, QPoint topLeft, const std::vector<ChipSegment>& segments, QPoint point);
} // namespace qvistools
