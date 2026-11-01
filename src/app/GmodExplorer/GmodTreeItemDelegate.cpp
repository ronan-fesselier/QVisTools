#include "GmodTreeItemDelegate.h"

#include "app/detail/QStringUtils.h"
#include "app/SdkQtTypes.h"
#include "GmodTreeModel.h"

#include <viscore/gmod/NodeCategory.h>

#include <dnv/vista/sdk/core/GmodNode.h>

#include <QPainter>

#include <algorithm>
#include <string_view>
#include <vector>

namespace qvistools
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
        if (node == nullptr)
        {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

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
            const auto category{ viscore::gmod::categoryOf(*skippedParentNode) };
            segments.push_back({ detail::toQString(skippedParentNode->code()),
                                 categoryColor(category),
                                 linkIcon(skippedParentNode->isMappable()) });
        }
        {
            const auto category{ viscore::gmod::categoryOf(*node) };
            segments.push_back(
                { detail::toQString(node->code()), categoryColor(category), linkIcon(node->isMappable()) });
        }
        if (productNode != nullptr)
        {
            const auto category{ viscore::gmod::categoryOf(*productNode) };
            segments.push_back({ detail::toQString(productNode->code()),
                                 categoryColor(category),
                                 linkIcon(productNode->isMappable()) });
        }

        return segments;
    }
} // namespace qvistools
