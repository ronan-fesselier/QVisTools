#include "GmodTreeItemDelegate.h"

#include "detail/StringUtils.h"
#include "gui/SdkQtTypes.h"
#include "GmodTreeModel.h"

#include <dnv/vista/sdk/core/GmodNode.h>

#include <QApplication>
#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QStyle>

#include <algorithm>
#include <string_view>
#include <vector>

namespace qvistools::gui
{
    namespace
    {
        constexpr int ChipHeight{ 18 };
        constexpr int ChipRadius{ 4 };
        constexpr int ChipPaddingH{ 6 };
        constexpr int ChipGroupSpacing{ 6 };
        constexpr int ChipVerticalMargin{ 4 };

        struct ChipSegment
        {
            QString text;
            QColor color;
        };

        QColor codeChipColor(const dnv::vista::sdk::GmodNode& node)
        {
            if (node.isRoot())
            {
                return QColor{ "#004d00" };
            }
            if (node.isLeafNode())
            {
                return node.isAssetFunctionNode() ? QColor{ "#00ff00" } : QColor{ "#ccffcc" };
            }
            if (node.isProductType())
            {
                return QColor{ "#ff0000" };
            }
            if (node.isFunctionComposition())
            {
                return QColor{ "#99cc00" };
            }

            if (node.metadata().type() == "GROUP")
            {
                return QColor{ "#008000" };
            }
            if (node.metadata().type() == "SELECTION")
            {
                return QColor{ "#ff9100" };
            }

            return QColor{ "#9b59b6" }; // Unhandled node category (visible marker)
        }

        int drawChipGroup(QPainter* painter, QPoint topLeft, const std::vector<ChipSegment>& segments)
        {
            const QFontMetrics metrics{ painter->fontMetrics() };

            std::vector<int> widths;
            widths.reserve(segments.size());
            int totalWidth{ 0 };
            for (const auto& segment : segments)
            {
                const int width{ metrics.horizontalAdvance(segment.text) + 2 * ChipPaddingH };
                widths.push_back(width);
                totalWidth += width;
            }

            const QRect groupRect{ topLeft, QSize{ totalWidth, ChipHeight } };

            QPainterPath outline;
            outline.addRoundedRect(groupRect, ChipRadius, ChipRadius);

            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setClipPath(outline);

            int x{ topLeft.x() };
            for (std::size_t i{ 0 }; i < segments.size(); ++i)
            {
                const QRect segmentRect{ QPoint{ x, topLeft.y() }, QSize{ widths[i], ChipHeight } };
                painter->fillRect(segmentRect, segments[i].color);
                painter->setPen(Qt::black);
                painter->drawText(segmentRect, Qt::AlignCenter, segments[i].text);
                x += widths[i];
            }

            painter->setClipping(false);
            painter->setPen(QPen{ QColor{ 0, 0, 0, 60 } });
            x = topLeft.x();
            for (std::size_t i{ 0 }; i + 1 < segments.size(); ++i)
            {
                x += widths[i];
                painter->drawLine(QPoint{ x, topLeft.y() }, QPoint{ x, topLeft.y() + ChipHeight });
            }

            painter->restore();

            return totalWidth;
        }
    } // namespace

    GmodTreeItemDelegate::GmodTreeItemDelegate(QObject* parent)
        : QStyledItemDelegate{ parent }
    {}

    void GmodTreeItemDelegate::paint(
        QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        const auto* node{ index.data(GmodTreeModel::NodeRole).value<const dnv::vista::sdk::GmodNode*>() };
        if (node == nullptr)
        {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        const auto* productNode{ index.data(GmodTreeModel::ProductNodeRole).value<const dnv::vista::sdk::GmodNode*>() };
        const auto* skippedParentNode{
            index.data(GmodTreeModel::SkippedParentNodeRole).value<const dnv::vista::sdk::GmodNode*>()
        };

        painter->save();

        QStyleOptionViewItem opt{ option };
        initStyleOption(&opt, index);
        opt.text.clear();

        if (opt.state & QStyle::State_Selected)
        {
            opt.palette.setColor(QPalette::HighlightedText, Qt::black);
        }

        QStyle* style{ opt.widget != nullptr ? opt.widget->style() : QApplication::style() };
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

        const auto& metadata{ node->metadata() };
        const auto name{ metadata.commonName().has_value() ? std::string_view(*metadata.commonName())
                                                           : metadata.name() };

        QPoint cursor{ option.rect.topLeft() };
        cursor.setY(cursor.y() + (option.rect.height() - ChipHeight) / 2);

        std::vector<ChipSegment> segments;
        if (skippedParentNode != nullptr)
        {
            segments.push_back({ detail::toQString(skippedParentNode->code()), codeChipColor(*skippedParentNode) });
        }
        segments.push_back({ detail::toQString(node->code()), codeChipColor(*node) });
        if (productNode != nullptr)
        {
            segments.push_back({ detail::toQString(productNode->code()), codeChipColor(*productNode) });
        }

        const auto groupWidth{ drawChipGroup(painter, cursor, segments) };
        cursor.setX(cursor.x() + groupWidth + ChipGroupSpacing);

        painter->setPen(
            opt.palette.color(opt.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text));
        const QRect nameRect{ cursor, QSize{ option.rect.right() - cursor.x(), option.rect.height() } };
        const auto elidedName{ opt.fontMetrics.elidedText(detail::toQString(name), Qt::ElideRight, nameRect.width()) };
        painter->drawText(nameRect, Qt::AlignVCenter, elidedName);

        painter->restore();
    }

    QSize GmodTreeItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        auto size{ QStyledItemDelegate::sizeHint(option, index) };
        size.setHeight(std::max(size.height(), ChipHeight + ChipVerticalMargin));
        return size;
    }
} // namespace qvistools::gui
