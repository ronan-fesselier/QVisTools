#include "GmodSearchResultDelegate.h"

#include "core/vis/gmod/AncestorPath.h"
#include "core/vis/gmod/NodeCategory.h"

#include "app/gui/detail/QStringUtils.h"
#include "app/gui/SdkQtTypes.h"
#include "app/gui/Shared/GmodChipRenderer.h"

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
    GmodSearchResultDelegate::GmodSearchResultDelegate(QObject* parent)
        : QStyledItemDelegate{ parent }
    {}

    void GmodSearchResultDelegate::paint(
        QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        const auto segments{ buildSegments(index) };
        if (segments.empty())
        {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        const auto* node{ index.data(NodeRole).value<const dnv::vista::sdk::GmodNode*>() };
        const auto& metadata{ node->metadata() };
        const auto name{ metadata.commonName().has_value() ? std::string_view(*metadata.commonName())
                                                           : metadata.name() };

        QStyleOptionViewItem opt{ option };
        initStyleOption(&opt, index);

        paintChipRow(painter, opt, detail::toQString(name), segments);
    }

    QSize GmodSearchResultDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        auto size{ QStyledItemDelegate::sizeHint(option, index) };
        size.setHeight(std::max(size.height(), CHIP_HEIGHT + CHIP_VERTICAL_MARGIN));
        return size;
    }

    bool GmodSearchResultDelegate::helpEvent(
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

    std::vector<ChipSegment> GmodSearchResultDelegate::buildSegments(const QModelIndex& index)
    {
        const auto* node{ index.data(NodeRole).value<const dnv::vista::sdk::GmodNode*>() };
        if (node == nullptr)
        {
            return {};
        }

        const auto* parent{ index.data(ParentRole).value<const dnv::vista::sdk::GmodNode*>() };

        std::vector<ChipSegment> segments;
        {
            for (const auto* ancestor : core::vis::shortAncestorPath(*parent))
            {
                const auto category{ core::vis::categoryOf(*ancestor) };
                segments.push_back({ detail::toQString(ancestor->code()), categoryColor(category), category });
            }
        }
        {
            const auto category{ core::vis::categoryOf(*node) };
            segments.push_back({ detail::toQString(node->code()), categoryColor(category), category });
        }

        return segments;
    }

    QPoint GmodSearchResultDelegate::chipTopLeft(const QRect& rect)
    {
        return QPoint{ rect.left(), rect.top() + (rect.height() - CHIP_HEIGHT) / 2 };
    }
} // namespace qvistools::gui
