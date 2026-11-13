#include "GmodTreeItemDelegate.h"

#include "detail/StringUtils.h"
#include "gui/SdkQtTypes.h"
#include "gui/Shared/GmodChipRenderer.h"
#include "GmodTreeModel.h"

#include <dnv/vista/sdk/core/GmodNode.h>

#include <QAbstractItemView>
#include <QApplication>
#include <QFontMetrics>
#include <QHelpEvent>
#include <QPainter>
#include <QStyle>
#include <QToolTip>

#include <algorithm>
#include <string_view>
#include <vector>

namespace qvistools::gui
{
    namespace
    {
        chip::Segment makeSegment(const dnv::vista::sdk::GmodNode& node)
        {
            const auto category{ chip::categoryOf(node) };
            return { detail::toQString(node.code()), chip::categoryColor(category), category };
        }

        std::vector<chip::Segment> buildSegments(const QModelIndex& index)
        {
            const auto* node{ index.data(GmodTreeModel::NodeRole).value<const dnv::vista::sdk::GmodNode*>() };
            if (node == nullptr)
            {
                return {};
            }

            const auto* productNode{
                index.data(GmodTreeModel::ProductNodeRole).value<const dnv::vista::sdk::GmodNode*>()
            };
            const auto* skippedParentNode{
                index.data(GmodTreeModel::SkippedParentNodeRole).value<const dnv::vista::sdk::GmodNode*>()
            };

            std::vector<chip::Segment> segments;
            if (skippedParentNode != nullptr)
            {
                segments.push_back(makeSegment(*skippedParentNode));
            }
            segments.push_back(makeSegment(*node));
            if (productNode != nullptr)
            {
                segments.push_back(makeSegment(*productNode));
            }

            return segments;
        }

        QPoint chipTopLeft(const QRect& itemRect)
        {
            QPoint topLeft{ itemRect.topLeft() };
            topLeft.setY(topLeft.y() + (itemRect.height() - chip::Height) / 2);
            return topLeft;
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

        QPoint cursor{ chipTopLeft(option.rect) };

        const auto segments{ buildSegments(index) };
        const auto groupWidth{ chip::drawChipGroup(painter, cursor, segments) };
        cursor.setX(cursor.x() + groupWidth + chip::GroupSpacing);

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
        size.setHeight(std::max(size.height(), chip::Height + chip::VerticalMargin));
        return size;
    }

    bool GmodTreeItemDelegate::helpEvent(
        QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& option, const QModelIndex& index)
    {
        const auto segments{ buildSegments(index) };
        if (segments.empty())
        {
            return QStyledItemDelegate::helpEvent(event, view, option, index);
        }

        const QFontMetrics metrics{ option.font };
        const auto hit{ chip::hitTest(metrics, chipTopLeft(option.rect), segments, event->pos()) };
        if (!hit.has_value())
        {
            return QStyledItemDelegate::helpEvent(event, view, option, index);
        }

        QToolTip::showText(event->globalPos(), chip::categoryName(segments[*hit].category), view);

        return true;
    }
} // namespace qvistools::gui
