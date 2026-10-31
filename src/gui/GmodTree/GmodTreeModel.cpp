#include "GmodTreeModel.h"

#include "detail/StringUtils.h"

#include <dnv/vista/sdk/core/Gmod.h>

#include <algorithm>
#include <cctype>

namespace qvistools::gui
{
    GmodTreeModel::GmodTreeModel(const dnv::vista::sdk::Gmod& gmod, QObject* parent)
        : QAbstractItemModel{ parent },
          m_rootNode{ &gmod.rootNode() }
    {}

    QModelIndex GmodTreeModel::index(int row, int column, const QModelIndex& parent) const
    {
        if (!hasIndex(row, column, parent))
        {
            return {};
        }

        if (!parent.isValid())
        {
            if (row != 0)
            {
                return {};
            }

            auto* handle{ handleFor(*m_rootNode, nullptr, parent) };
            return createIndex(row, column, handle);
        }

        const auto* parentHandle{ static_cast<NodeHandle*>(parent.internalPointer()) };
        const auto* parentNode{ parentHandle->node };

        const auto& children{ sortedChildren(*parentNode) };
        if (row < 0 || static_cast<size_t>(row) >= children.size())
        {
            return {};
        }

        auto* handle{ handleFor(*children[static_cast<size_t>(row)], parentNode, parent) };

        return createIndex(row, column, handle);
    }

    QModelIndex GmodTreeModel::parent(const QModelIndex& index) const
    {
        if (!index.isValid())
        {
            return {};
        }

        const auto* handle{ static_cast<NodeHandle*>(index.internalPointer()) };

        return handle->parentIndex;
    }

    int GmodTreeModel::rowCount(const QModelIndex& parent) const
    {
        if (parent.column() > 0)
        {
            return 0;
        }

        if (!parent.isValid())
        {
            return 1; // Root level: VE is the model's single top-level node
        }

        const auto* handle{ static_cast<NodeHandle*>(parent.internalPointer()) };
        return static_cast<int>(sortedChildren(*handle->node).size());
    }

    int GmodTreeModel::columnCount(const QModelIndex& /*parent*/) const
    {
        return 1;
    }

    QVariant GmodTreeModel::data(const QModelIndex& index, int role) const
    {
        if (!index.isValid())
        {
            return {};
        }

        const auto* handle{ static_cast<NodeHandle*>(index.internalPointer()) };
        const auto* node{ handle->node };

        const auto code{ node->code() };

        const dnv::vista::sdk::GmodNode* productNode{ nullptr };
        if (const auto productType{ node->productType() }; productType.has_value())
        {
            productNode = *productType;
        }

        const auto& metadata = productNode != nullptr ? productNode->metadata() : node->metadata();
        const auto name{ metadata.commonName().has_value() ? std::string_view(*metadata.commonName())
                                                           : metadata.name() };

        switch (role)
        {
            case Qt::DisplayRole:
            {
                return QStringLiteral("%1 - %2").arg(detail::toQString(code), detail::toQString(name));
            }
            case NodeRole:
                return QVariant::fromValue(node);
            case MappableRole:
                return node->isMappable();
            case ProductNodeRole:
                return QVariant::fromValue(productNode);
            case SkippedParentNodeRole:
            {
                const auto* parentNode{ handle->parentNode };
                if (parentNode == nullptr || !parentNode->productSelection().has_value())
                {
                    return {};
                }

                return QVariant::fromValue(parentNode);
            }
            default:
                return {};
        }
    }

    GmodTreeModel::NodeHandle* GmodTreeModel::handleFor(
        const dnv::vista::sdk::GmodNode& node,
        const dnv::vista::sdk::GmodNode* parentNode,
        const QModelIndex& parentIndex) const
    {
        const auto key{ std::make_pair(&node, QPersistentModelIndex{ parentIndex }) };

        auto it{ m_handles.find(key) };
        if (it == m_handles.end())
        {
            auto handle{ std::make_unique<NodeHandle>(NodeHandle{ &node, parentNode, parentIndex }) };
            it = m_handles.emplace(key, std::move(handle)).first;
        }

        return it->second.get();
    }

    const std::vector<const dnv::vista::sdk::GmodNode*>& GmodTreeModel::sortedChildren(
        const dnv::vista::sdk::GmodNode& node) const
    {
        const auto* effectiveNode{ &node };
        if (const auto productType{ node.productType() }; productType.has_value())
        {
            effectiveNode = *productType;
        }
        else if (const auto productSelection{ node.productSelection() }; productSelection.has_value())
        {
            effectiveNode = *productSelection;
        }

        auto it{ m_sortedChildrenCache.find(effectiveNode) };
        if (it != m_sortedChildrenCache.end())
        {
            return it->second;
        }

        std::vector<const dnv::vista::sdk::GmodNode*> children(
            effectiveNode->children().begin(), effectiveNode->children().end());

        std::stable_sort(
            children.begin(),
            children.end(),
            [](const dnv::vista::sdk::GmodNode* a, const dnv::vista::sdk::GmodNode* b) {
                return detail::naturalCompare(a->code(), b->code()) < 0;
            });

        return m_sortedChildrenCache.emplace(effectiveNode, std::move(children)).first->second;
    }
} // namespace qvistools::gui
