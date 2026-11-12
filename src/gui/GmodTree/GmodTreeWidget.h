#pragma once

#include <dnv/vista/sdk/core/GmodPath.h>

#include <QWidget>

#include <optional>

class QEvent;
class QTreeView;

namespace dnv::vista::sdk
{
    class Gmod;
    class GmodNode;
} // namespace dnv::vista::sdk

namespace qvistools::gui
{
    class GmodTreeModel;

    class GmodTreeWidget final : public QWidget
    {
        Q_OBJECT

    public:
        explicit GmodTreeWidget(const dnv::vista::sdk::Gmod& gmod, QWidget* parent = nullptr);

        void setGmod(const dnv::vista::sdk::Gmod& gmod);

        [[nodiscard]] const dnv::vista::sdk::GmodNode* currentNode() const;
        [[nodiscard]] std::optional<dnv::vista::sdk::GmodPath> currentPath() const;

        void navigateTo(const dnv::vista::sdk::GmodNode& node);

        void collapseAll();

    protected:
        virtual bool eventFilter(QObject* watched, QEvent* event) override;

    private:
        void setupTreeView();
        void setupLayout();
        void setupConnections();

        void expandRoot();

        void onRootCollapsed(const QModelIndex& index);
        void onCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous);

        GmodTreeModel* m_model;
        QTreeView* m_treeView;

    signals:
        void nodeSelectionChanged(const dnv::vista::sdk::GmodNode* node);
    };
} // namespace qvistools::gui
