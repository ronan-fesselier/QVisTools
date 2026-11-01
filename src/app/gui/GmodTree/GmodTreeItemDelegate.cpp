#include "GmodTreeItemDelegate.h"

#include "core/vis/gmod/NodeCategory.h"

#include "app/gui/detail/QStringUtils.h"
#include "app/gui/SdkQtTypes.h"
#include "app/gui/Shared/GmodChipRenderer.h"
#include "GmodTreeModel.h"

#include <dnv/vista/sdk/core/GmodNode.h>

#include <QAbstractItemView>
#include <QHelpEvent>
#include <QPainter>
#include <QToolTip>

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
        const auto segments{ buildSegments(index) };
        if (segments.empty())
        {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        const auto* node{ index.data(GmodTreeModel::NodeRole).value<const dnv::vista::sdk::GmodNode*>() };
        const auto& metadata{ node->metadata() };
        const auto name{ metadata.commonName().has_value() ? std::string_view(*metadata.commonName())
                                                           : metadata.name() };

        QStyleOptionViewItem opt{ option };
        initStyleOption(&opt, index);

        paintChipRow(painter, opt, detail::toQString(name), segments);
    }

    QSize GmodTreeItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        auto size{ QStyledItemDelegate::sizeHint(option, index) };
        size.setHeight(std::max(size.height(), CHIP_HEIGHT + CHIP_VERTICAL_MARGIN));
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
        const auto hit{ hitTest(metrics, chipTopLeft(option.rect), segments, event->pos()) };
        if (!hit.has_value())
        {
            return QStyledItemDelegate::helpEvent(event, view, option, index);
        }

        QToolTip::showText(
            event->globalPos(), detail::toQString(core::vis::categoryName(segments[*hit].category)), view);
        return true;
    }

    std::vector<ChipSegment> GmodTreeItemDelegate::buildSegments(const QModelIndex& index)
    {
        const auto* node{ index.data(GmodTreeModel::NodeRole).value<const dnv::vista::sdk::GmodNode*>() };
        if (node == nullptr)
        {
            return {};
        }

        const auto* productNode{ index.data(GmodTreeModel::ProductNodeRole).value<const dnv::vista::sdk::GmodNode*>() };
        const auto* skippedParentNode{
            index.data(GmodTreeModel::SkippedParentNodeRole).value<const dnv::vista::sdk::GmodNode*>()
        };

        const auto linkIcon{ [](bool mappable) { return mappable ? ChipIcon::Link : ChipIcon::None; } };

        std::vector<ChipSegment> segments;
        if (skippedParentNode != nullptr)
        {
            const auto category{ core::vis::categoryOf(*skippedParentNode) };
            segments.push_back({ detail::toQString(skippedParentNode->code()),
                                 categoryColor(category),
                                 category,
                                 linkIcon(skippedParentNode->isMappable()) });
        }
        {
            const auto category{ core::vis::categoryOf(*node) };
            segments.push_back(
                { detail::toQString(node->code()), categoryColor(category), category, linkIcon(node->isMappable()) });
        }
        if (productNode != nullptr)
        {
            const auto category{ core::vis::categoryOf(*productNode) };
            segments.push_back({ detail::toQString(productNode->code()),
                                 categoryColor(category),
                                 category,
                                 linkIcon(productNode->isMappable()) });
        }

        return segments;
    }

    QPoint GmodTreeItemDelegate::chipTopLeft(const QRect& rect)
    {
        return QPoint{ rect.left(), rect.top() + (rect.height() - CHIP_HEIGHT) / 2 };
    }
} // namespace qvistools::gui
