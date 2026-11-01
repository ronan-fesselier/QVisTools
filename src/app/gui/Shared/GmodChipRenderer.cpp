#include "GmodChipRenderer.h"

#include <QApplication>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QStyle>
#include <QStyledItemDelegate>

#include <algorithm>

namespace qvistools::gui
{
    namespace
    {
        constexpr int LINK_ICON_SIZE{ 10 };
        constexpr int LINK_ICON_SPACING{ 4 };

        const QPixmap& chipIconPixmap(ChipIcon icon)
        {
            static const QPixmap linkPixmap{ QStringLiteral(":/icons/link.png") };
            static const QPixmap unlinkPixmap{ QStringLiteral(":/icons/unlink.png") };
            return icon == ChipIcon::Link ? linkPixmap : unlinkPixmap;
        }

        void drawChipIcon(QPainter* painter, QRect iconRect, ChipIcon icon)
        {
            if (icon == ChipIcon::None)
            {
                return;
            }

            const auto& pixmap{ chipIconPixmap(icon) };
            painter->drawPixmap(
                iconRect,
                pixmap.scaled(
                    iconRect.size() * painter->device()->devicePixelRatio(),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation));
        }
    } // namespace

    QColor categoryColor(core::vis::NodeCategory category)
    {
        switch (category)
        {
            case core::vis::NodeCategory::Root:
                return QColor{ "#004d00" };
            case core::vis::NodeCategory::AssetFunctionLeaf:
                return QColor{ "#00ff00" };
            case core::vis::NodeCategory::OtherLeaf:
                return QColor{ "#ccffcc" };
            case core::vis::NodeCategory::ProductType:
                return QColor{ "#ff0000" };
            case core::vis::NodeCategory::FunctionComposition:
                return QColor{ "#99cc00" };
            case core::vis::NodeCategory::AssetFunctionGroup:
            case core::vis::NodeCategory::ProductFunctionGroup:
                return QColor{ "#008000" };
            case core::vis::NodeCategory::FunctionSelection:
            case core::vis::NodeCategory::ProductSelection:
                return QColor{ "#ff9100" };
            case core::vis::NodeCategory::Other:
                break;
        }

        return QColor{ "#9b59b6" }; // Unhandled node category (visible marker)
    }

    QColor readableTextColor(const QColor& background)
    {
        const double yiq{ (background.red() * 299 + background.green() * 587 + background.blue() * 114) / 1000.0 };
        return yiq >= 128.0 ? QColor{ Qt::black } : QColor{ Qt::white };
    }

    int segmentWidth(const QFontMetrics& metrics, const ChipSegment& segment)
    {
        int width{ metrics.horizontalAdvance(segment.text) + 2 * CHIP_PADDING_H };
        if (segment.icon != ChipIcon::None)
        {
            width += LINK_ICON_SIZE + LINK_ICON_SPACING;
        }
        return width;
    }

    int drawChipGroup(QPainter* painter, QPoint topLeft, const std::vector<ChipSegment>& segments)
    {
        const QFontMetrics metrics{ painter->fontMetrics() };

        std::vector<int> widths;
        widths.reserve(segments.size());
        int totalWidth{ 0 };
        for (const auto& segment : segments)
        {
            const int width{ segmentWidth(metrics, segment) };
            widths.push_back(width);
            totalWidth += width;
        }

        const QRect groupRect{ topLeft, QSize{ totalWidth, CHIP_HEIGHT } };

        QPainterPath outline;
        outline.addRoundedRect(groupRect, CHIP_RADIUS, CHIP_RADIUS);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setClipPath(outline);

        int x{ topLeft.x() };
        for (std::size_t i{ 0 }; i < segments.size(); ++i)
        {
            const QRect segmentRect{ QPoint{ x, topLeft.y() }, QSize{ widths[i], CHIP_HEIGHT } };
            painter->fillRect(segmentRect, segments[i].color);

            const auto textColor{ readableTextColor(segments[i].color) };
            painter->setPen(textColor);

            if (segments[i].icon != ChipIcon::None)
            {
                const QRect iconRect{ segmentRect.right() - CHIP_PADDING_H - LINK_ICON_SIZE,
                                      segmentRect.top() + (segmentRect.height() - LINK_ICON_SIZE) / 2,
                                      LINK_ICON_SIZE,
                                      LINK_ICON_SIZE };
                drawChipIcon(painter, iconRect, segments[i].icon);

                const QRect textRect{ segmentRect.left(),
                                      segmentRect.top(),
                                      segmentRect.width() - LINK_ICON_SIZE - LINK_ICON_SPACING,
                                      segmentRect.height() };
                painter->drawText(textRect, Qt::AlignCenter, segments[i].text);
            }
            else
            {
                painter->drawText(segmentRect, Qt::AlignCenter, segments[i].text);
            }

            x += widths[i];
        }

        painter->setClipping(false);
        painter->setPen(QPen{ QColor{ 0, 0, 0, 60 } });
        x = topLeft.x();
        for (std::size_t i{ 0 }; i + 1 < segments.size(); ++i)
        {
            x += widths[i];
            painter->drawLine(QPoint{ x, topLeft.y() }, QPoint{ x, topLeft.y() + CHIP_HEIGHT });
        }

        painter->restore();

        return totalWidth;
    }

    void paintChipRow(
        QPainter* painter,
        const QStyleOptionViewItem& initializedOption,
        const QString& name,
        const std::vector<ChipSegment>& segments)
    {
        painter->save();

        QStyleOptionViewItem opt{ initializedOption };
        opt.text.clear();

        if (opt.state & QStyle::State_Selected)
        {
            opt.palette.setColor(QPalette::HighlightedText, readableTextColor(opt.palette.color(QPalette::Highlight)));
        }

        QStyle* style{ opt.widget != nullptr ? opt.widget->style() : QApplication::style() };
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

        QPoint cursor{ opt.rect.topLeft() };
        cursor.setY(cursor.y() + (opt.rect.height() - CHIP_HEIGHT) / 2);

        const auto groupWidth{ drawChipGroup(painter, cursor, segments) };
        cursor.setX(cursor.x() + groupWidth + CHIP_GROUP_SPACING);

        painter->setPen(
            opt.palette.color(opt.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text));
        const QRect nameRect{ cursor, QSize{ std::max(0, opt.rect.right() + 1 - cursor.x()), opt.rect.height() } };
        const auto elidedName{ opt.fontMetrics.elidedText(name, Qt::ElideRight, nameRect.width()) };
        painter->drawText(nameRect, Qt::AlignVCenter, elidedName);

        painter->restore();
    }

    std::optional<std::size_t> hitTest(
        const QFontMetrics& metrics, QPoint topLeft, const std::vector<ChipSegment>& segments, QPoint point)
    {
        if (point.y() < topLeft.y() || point.y() >= topLeft.y() + CHIP_HEIGHT)
        {
            return std::nullopt;
        }

        int x{ topLeft.x() };
        for (std::size_t i{ 0 }; i < segments.size(); ++i)
        {
            const int width{ segmentWidth(metrics, segments[i]) };
            if (point.x() >= x && point.x() < x + width)
            {
                return i;
            }
            x += width;
        }

        return std::nullopt;
    }
} // namespace qvistools::gui
