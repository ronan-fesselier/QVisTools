#include "GmodTreeModel.h"

#include "app/detail/QStringUtils.h"

#include <viscore/gmod/GmodNodeResolution.h>

#include <dnv/vista/sdk/core/Gmod.h>
#include <dnv/vista/sdk/core/GmodNode.h>

namespace qvistools
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

            auto* handle{ handleFor(*m_rootNode, parent) };
            return createIndex(row, column, handle);
        }

        const auto* parentHandle{ static_cast<NodeHandle*>(parent.internalPointer()) };
        const auto* parentNode{ parentHandle->node };

        const auto& children{ cachedSortedChildren(*parentNode) };
        if (row < 0 || static_cast<size_t>(row) >= children.size())
        {
            return {};
        }

        auto* handle{ handleFor(*children[static_cast<size_t>(row)], parent) };

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
            default:
                return {};
        }
    }

    GmodTreeModel::NodeHandle* GmodTreeModel::handleFor(
        const dnv::vista::sdk::GmodNode& node, const QModelIndex& parentIndex) const
    {
        const auto key{ std::make_pair(&node, QPersistentModelIndex{ parentIndex }) };

        auto it{ m_handles.find(key) };
        if (it == m_handles.end())
        {
            auto handle{ std::make_unique<NodeHandle>(NodeHandle{ &node, parentIndex }) };
            it = m_handles.emplace(key, std::move(handle)).first;
        }

        return it->second.get();
    }

    const std::vector<const dnv::vista::sdk::GmodNode*>& GmodTreeModel::cachedSortedChildren(
        const dnv::vista::sdk::GmodNode& node) const
    {
        auto it{ m_sortedChildrenCache.find(&node) };
        if (it != m_sortedChildrenCache.end())
        {
            return it->second;
        }

        return m_sortedChildrenCache.emplace(&node, viscore::gmod::sortedChildren(node)).first->second;
    }
} // namespace qvistools
