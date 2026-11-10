#include "GmodTreeItemDelegate.h"

#include "detail/StringUtils.h"
#include "gui/SdkQtTypes.h"
#include "GmodChipRenderer.h"
#include "GmodTreeModel.h"

#include <dnv/vista/sdk/core/GmodNode.h>

#include <QApplication>
#include <QPainter>
#include <QStyle>

#include <algorithm>
#include <string_view>
#include <vector>

namespace qvistools::gui
{
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
        cursor.setY(cursor.y() + (option.rect.height() - chip::Height) / 2);

        std::vector<chip::Segment> segments;
        if (skippedParentNode != nullptr)
        {
            segments.push_back(
                { detail::toQString(skippedParentNode->code()), chip::codeChipColor(*skippedParentNode) });
        }
        segments.push_back({ detail::toQString(node->code()), chip::codeChipColor(*node) });
        if (productNode != nullptr)
        {
            segments.push_back({ detail::toQString(productNode->code()), chip::codeChipColor(*productNode) });
        }

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
} // namespace qvistools::gui
