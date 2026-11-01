#include "GmodSearchResultDelegate.h"

#include "app/detail/QStringUtils.h"
#include "app/shared/GmodChipRenderer.h"
#include "app/SdkQtTypes.h"

#include <viscore/gmod/AncestorPath.h>
#include <viscore/gmod/NodeCategory.h>

#include <dnv/vista/sdk/core/GmodNode.h>

#include <QPainter>

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
        const auto* node{ index.data(NodeRole).value<const dnv::vista::sdk::GmodNode*>() };
        if (node == nullptr)
        {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        const auto* parent{ index.data(ParentRole).value<const dnv::vista::sdk::GmodNode*>() };

        const auto& metadata{ node->metadata() };
        const auto name{ metadata.commonName().has_value() ? std::string_view(*metadata.commonName())
                                                           : metadata.name() };

        std::vector<ChipSegment> segments;
        if (parent != nullptr)
        {
            for (const auto* ancestor : viscore::gmod::shortAncestorPath(*parent))
            {
                segments.push_back(
                    { detail::toQString(ancestor->code()), categoryColor(viscore::gmod::categoryOf(*ancestor)) });
            }
        }
        segments.push_back({ detail::toQString(node->code()), categoryColor(viscore::gmod::categoryOf(*node)) });

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
} // namespace qvistools
