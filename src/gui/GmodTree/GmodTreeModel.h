#pragma once

#include "gui/SdkQtTypes.h"

#include <QAbstractItemModel>

#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace dnv::vista::sdk
{
    class Gmod;
} // namespace dnv::vista::sdk

namespace qvistools::gui
{
    class GmodTreeModel final : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        enum Role
        {
            NodeRole = Qt::UserRole + 1,
            MappableRole,
            ProductNodeRole,
            SkippedParentNodeRole
        };

        explicit GmodTreeModel(const dnv::vista::sdk::Gmod& gmod, QObject* parent = nullptr);

        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
        virtual QModelIndex parent(const QModelIndex& index) const override;
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    private:
        struct NodeHandle
        {
            const dnv::vista::sdk::GmodNode* node;
            const dnv::vista::sdk::GmodNode* parentNode;
            QModelIndex parentIndex;
        };

        NodeHandle* handleFor(
            const dnv::vista::sdk::GmodNode& node,
            const dnv::vista::sdk::GmodNode* parentNode,
            const QModelIndex& parentIndex) const;

        const std::vector<const dnv::vista::sdk::GmodNode*>& sortedChildren(
            const dnv::vista::sdk::GmodNode& node) const;

        const dnv::vista::sdk::GmodNode* m_rootNode;

        mutable std::
            map<std::pair<const dnv::vista::sdk::GmodNode*, QPersistentModelIndex>, std::unique_ptr<NodeHandle>>
                m_handles;
        mutable std::map<const dnv::vista::sdk::GmodNode*, std::vector<const dnv::vista::sdk::GmodNode*>>
            m_sortedChildrenCache;
    };
} // namespace qvistools::gui
