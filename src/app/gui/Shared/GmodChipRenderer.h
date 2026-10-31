#pragma once

#include "core/vis/gmod/NodeCategory.h"

#include <QColor>
#include <QModelIndex>
#include <QPoint>
#include <QString>

#include <optional>
#include <vector>

class QFontMetrics;
class QPainter;
class QStyleOptionViewItem;

namespace qvistools::gui
{
    constexpr int CHIP_HEIGHT{ 18 };
    constexpr int CHIP_RADIUS{ CHIP_HEIGHT / 2 };
    constexpr int CHIP_PADDING_H{ 6 };
    constexpr int CHIP_GROUP_SPACING{ 6 };
    constexpr int CHIP_VERTICAL_MARGIN{ 4 };

    /**
     * @brief Map a Gmod node category to its display color
     */
    [[nodiscard]] QColor categoryColor(core::vis::NodeCategory category);

    /**
     * @brief Pick a readable text color (black or white) for a given chip background
     * @details Uses the YIQ formula to estimate perceived brightness
     */
    [[nodiscard]] QColor readableTextColor(const QColor& background);

    /**
     * @brief Which badge icon, if any, a chip segment shows
     */
    enum class ChipIcon
    {
        None,
        Link,
        Unlink
    };

    struct ChipSegment
    {
        QString text;
        QColor color;
        ChipIcon icon{ ChipIcon::None };
    };

    /**
     * @brief Compute the painted width of a single chip segment, including its icon
     *        badge if any
     */
    [[nodiscard]] int segmentWidth(const QFontMetrics& metrics, const ChipSegment& segment);

    /**
     * @brief Paint a group of chip segments side by side, with a shared rounded outline
     * @return The total painted width, in pixels
     */
    int drawChipGroup(QPainter* painter, QPoint topLeft, const std::vector<ChipSegment>& segments);

    /**
     * @brief Paint a standard row: selection/background, a chip group, then an elided name
     * @details Shared by every delegate that renders a Gmod node as chips followed by its
     *          display name (GmodTreeItemDelegate, GmodSearchResultDelegate). The caller
     *          must pass an already-initialized style option (via initStyleOption()), and
     *          is only responsible for building the chip segments and resolving the name
     */
    void paintChipRow(
        QPainter* painter,
        const QStyleOptionViewItem& initializedOption,
        const QString& name,
        const std::vector<ChipSegment>& segments);
} // namespace qvistools::gui
