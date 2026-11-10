#include "GmodSearchResultDelegate.h"

#include "core/GmodPathUtils.h"
#include "detail/StringUtils.h"
#include "gui/SdkQtTypes.h"
#include "GmodChipRenderer.h"

#include <dnv/vista/sdk/core/GmodNode.h>

#include <QApplication>
#include <QPainter>
#include <QStyle>

#include <algorithm>
#include <string_view>
#include <vector>

namespace qvistools::gui
{
    GmodSearchResultDelegate::GmodSearchResultDelegate(QObject* parent)
        : QStyledItemDelegate{ parent }
    {}

    void GmodSearchResultDelegate::paint(
        QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        const auto* node{ index.data(NodeRole).value<const dnv::vista::sdk::GmodNode*>() };
        if (node == nullptr)
        {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        const auto* parent{ index.data(ParentRole).value<const dnv::vista::sdk::GmodNode*>() };
        const auto* skippedParent{ index.data(SkippedParentRole).value<const dnv::vista::sdk::GmodNode*>() };

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

        const auto* namedNode{ skippedParent != nullptr ? parent : node };
        const auto& metadata{ namedNode->metadata() };
        const auto name{ metadata.commonName().has_value() ? std::string_view(*metadata.commonName())
                                                           : metadata.name() };

        QPoint cursor{ option.rect.topLeft() };
        cursor.setY(cursor.y() + (option.rect.height() - chip::Height) / 2);

        std::vector<chip::Segment> segments;
        if (parent != nullptr)
        {
            for (const auto* ancestor : core::shortAncestorPath(*parent))
            {
                segments.push_back({ detail::toQString(ancestor->code()), chip::codeChipColor(*ancestor) });
            }
        }
        segments.push_back({ detail::toQString(node->code()), chip::codeChipColor(*node) });

        const auto groupWidth{ chip::drawChipGroup(painter, cursor, segments) };
        cursor.setX(cursor.x() + groupWidth + chip::GroupSpacing);

        painter->setPen(
            opt.palette.color(opt.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text));
        const QRect nameRect{ cursor, QSize{ option.rect.right() - cursor.x(), option.rect.height() } };
        const auto elidedName{ opt.fontMetrics.elidedText(detail::toQString(name), Qt::ElideRight, nameRect.width()) };
        painter->drawText(nameRect, Qt::AlignVCenter, elidedName);

        painter->restore();
    }

    QSize GmodSearchResultDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        auto size{ QStyledItemDelegate::sizeHint(option, index) };
        size.setHeight(std::max(size.height(), chip::Height + chip::VerticalMargin));
        return size;
    }
} // namespace qvistools::gui
