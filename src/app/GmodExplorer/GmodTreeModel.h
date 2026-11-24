#pragma once

#include "app/SdkQtTypes.h"

#include <viscore/gmod/SearchIndex.h>

#include <QAbstractItemModel>
#include <QHash>

#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace dnv::vista::sdk
{
    class Gmod;
    class GmodPath;
} // namespace dnv::vista::sdk

namespace qvistools
{
    class GmodTreeModel final : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        enum Role
        {
            NodeRole = Qt::UserRole + 1,
            ProductNodeRole,
            SkippedParentNodeRole
        };

        explicit GmodTreeModel(const dnv::vista::sdk::Gmod& gmod, QObject* parent = nullptr);

        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
        virtual QModelIndex parent(const QModelIndex& index) const override;
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        [[nodiscard]] QModelIndex indexForMatch(const viscore::gmod::SearchIndex::Match& match) const;
        [[nodiscard]] QModelIndex indexForPath(const dnv::vista::sdk::GmodPath& path) const;
        [[nodiscard]] std::optional<dnv::vista::sdk::GmodPath> pathForIndex(const QModelIndex& index) const;

    private:
        struct NodeHandle
        {
            const dnv::vista::sdk::GmodNode* node;
            const dnv::vista::sdk::GmodNode* parentNode;
            QPersistentModelIndex parentIndex;
        };

        NodeHandle* handleFor(
            const dnv::vista::sdk::GmodNode& node,
            const dnv::vista::sdk::GmodNode* parentNode,
            const QModelIndex& parentIndex) const;

        [[nodiscard]] QModelIndex indexForDisplayedPath(
            const std::vector<const dnv::vista::sdk::GmodNode*>& displayedPath) const;

        const std::vector<const dnv::vista::sdk::GmodNode*>& cachedSortedChildren(
            const dnv::vista::sdk::GmodNode& node) const;

        const dnv::vista::sdk::Gmod* m_gmod;
        const dnv::vista::sdk::GmodNode* m_rootNode;

        struct HandleKeyHash
        {
            std::size_t operator()(
                const std::pair<const dnv::vista::sdk::GmodNode*, QPersistentModelIndex>& key) const noexcept
            {
                return qHashMulti(0, key.first, key.second);
            }
        };

        mutable std::unordered_map<
            std::pair<const dnv::vista::sdk::GmodNode*, QPersistentModelIndex>,
            std::unique_ptr<NodeHandle>,
            HandleKeyHash>
            m_handles;
        mutable std::unordered_map<const dnv::vista::sdk::GmodNode*, std::vector<const dnv::vista::sdk::GmodNode*>>
            m_sortedChildrenCache;
    };
} // namespace qvistools
