#include "GmodTreeModel.h"

#include "core/vis/gmod/GmodNodeChildren.h"

#include "app/gui/detail/QStringUtils.h"

#include <dnv/vista/sdk/core/Gmod.h>
#include <dnv/vista/sdk/core/GmodNode.h>

#include <algorithm>

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

        const auto& children{ cachedSortedChildren(*parentNode) };
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
        return static_cast<int>(cachedSortedChildren(*handle->node).size());
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

        switch (role)
        {
            case Qt::DisplayRole:
            {
                const auto& metadata{ node->metadata() };
                const auto name{ metadata.commonName().has_value() ? std::string_view(*metadata.commonName())
                                                                   : metadata.name() };

                return QStringLiteral("%1 - %2").arg(detail::toQString(node->code()), detail::toQString(name));
            }
            case NodeRole:
                return QVariant::fromValue(node);
            case ProductNodeRole:
            {
                const auto productType{ node->productType() };
                return QVariant::fromValue(productType.has_value() ? *productType : nullptr);
            }
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

    QModelIndex GmodTreeModel::indexForMatch(const core::vis::SearchIndex::Match& match) const
    {
        if (match.node == nullptr)
        {
            return {};
        }

        const auto* entryNode{ match.parent };

        if (entryNode == nullptr)
        {
            // The match is the root itself: no displayed parent to walk down from
            return match.node == m_rootNode ? index(0, 0, {}) : QModelIndex{};
        }

        std::vector<const dnv::vista::sdk::GmodNode*> path{ entryNode };
        while (path.back() != m_rootNode)
        {
            const auto* current{ path.back() };

            const dnv::vista::sdk::GmodNode* displayedParent{ nullptr };
            for (const auto* candidate : current->parents())
            {
                const auto* resolved{ core::vis::effectiveNode(*candidate) };
                if (resolved == candidate || resolved == current)
                {
                    displayedParent = candidate;
                    break;
                }
            }

            if (displayedParent == nullptr)
            {
                return {};
            }
            path.push_back(displayedParent);
        }
        std::reverse(path.begin(), path.end());

        QModelIndex current{ index(0, 0, {}) };
        for (std::size_t i{ 1 }; i < path.size(); ++i)
        {
            const auto* parentNode{ path[i - 1] };
            const auto* childNode{ path[i] };

            if (core::vis::effectiveNode(*parentNode) == childNode)
            {
                continue;
            }

            const auto& children{ cachedSortedChildren(*parentNode) };
            const auto it{ std::find(children.begin(), children.end(), childNode) };
            if (it == children.end())
            {
                return {};
            }

            current = index(static_cast<int>(it - children.begin()), 0, current);
            if (!current.isValid())
            {
                return {};
            }
        }

        if (core::vis::effectiveNode(*entryNode) == match.node)
        {
            return current;
        }

        const auto& entryChildren{ cachedSortedChildren(*entryNode) };
        const auto nodeIt{ std::find(entryChildren.begin(), entryChildren.end(), match.node) };
        if (nodeIt == entryChildren.end())
        {
            return {};
        }

        return index(static_cast<int>(nodeIt - entryChildren.begin()), 0, current);
    }

    GmodTreeModel::NodeHandle* GmodTreeModel::handleFor(
        const dnv::vista::sdk::GmodNode& node,
        const dnv::vista::sdk::GmodNode* parentNode,
        const QModelIndex& parentIndex) const
    {
        QPersistentModelIndex persistentParentIndex{ parentIndex };
        const auto key{ std::make_pair(&node, persistentParentIndex) };

        auto it{ m_handles.find(key) };
        if (it == m_handles.end())
        {
            auto handle{ std::make_unique<NodeHandle>(
                NodeHandle{ &node, parentNode, std::move(persistentParentIndex) }) };
            it = m_handles.emplace(key, std::move(handle)).first;
        }

        return it->second.get();
    }

    const std::vector<const dnv::vista::sdk::GmodNode*>& GmodTreeModel::cachedSortedChildren(
        const dnv::vista::sdk::GmodNode& node) const
    {
        const auto* effectiveNode{ core::vis::effectiveNode(node) };

        auto it{ m_sortedChildrenCache.find(effectiveNode) };
        if (it != m_sortedChildrenCache.end())
        {
            return it->second;
        }

        return m_sortedChildrenCache.emplace(effectiveNode, core::vis::sortedChildren(*effectiveNode)).first->second;
    }
} // namespace qvistools::gui
