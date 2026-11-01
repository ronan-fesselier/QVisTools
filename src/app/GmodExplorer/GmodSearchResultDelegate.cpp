#include "GmodSearchResultDelegate.h"

#include "app/detail/QStringUtils.h"
#include "app/shared/GmodChipRenderer.h"
#include "app/SdkQtTypes.h"

#include <viscore/gmod/AncestorPath.h>
#include <viscore/gmod/NodeCategory.h>

#include <dnv/vista/sdk/core/GmodNode.h>

#include <QAbstractItemView>
#include <QHelpEvent>
#include <QPainter>
#include <QToolTip>

#include <algorithm>
#include <string_view>
#include <vector>

namespace qvistools
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
            event->globalPos(), detail::toQString(viscore::gmod::categoryName(segments[*hit].category)), view);
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
        if (parent != nullptr)
        {
            for (const auto* ancestor : viscore::gmod::shortAncestorPath(*parent))
            {
                const auto category{ viscore::gmod::categoryOf(*ancestor) };
                segments.push_back({ detail::toQString(ancestor->code()), categoryColor(category), category });
            }
        }
        {
            const auto category{ viscore::gmod::categoryOf(*node) };
            segments.push_back({ detail::toQString(node->code()), categoryColor(category), category });
        }

        return segments;
    }

    QPoint GmodSearchResultDelegate::chipTopLeft(const QRect& rect)
    {
        return QPoint{ rect.left(), rect.top() + (rect.height() - CHIP_HEIGHT) / 2 };
    }
} // namespace qvistools
