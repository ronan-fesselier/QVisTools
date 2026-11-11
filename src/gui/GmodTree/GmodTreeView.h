#pragma once

#include <QWidget>

class QComboBox;
class QEvent;
class QPushButton;
class QToolButton;
class QTreeView;

namespace dnv::vista::sdk
{
    class Gmod;
    class GmodNode;
} // namespace dnv::vista::sdk

namespace qvistools::gui
{
    class GmodSearchBar;
    class GmodTreeModel;

    class GmodTreeView final : public QWidget
    {
        Q_OBJECT

    public:
        explicit GmodTreeView(const dnv::vista::sdk::Gmod& gmod, QWidget* parent = nullptr);

    protected:
        virtual bool eventFilter(QObject* watched, QEvent* event) override;

    private:
        void setupTreeView();
        void setupVersionCombo(const dnv::vista::sdk::Gmod& gmod);
        void setupLayout();
        void setupConnections();

        void expandRoot();
        void collapseAll();

        void onVersionChanged(int index);
        void onRootCollapsed(const QModelIndex& index);
        void onSearchNodeActivated(const dnv::vista::sdk::GmodNode& node);

        const dnv::vista::sdk::Gmod* m_gmod;

        GmodTreeModel* m_model;
        QTreeView* m_treeView;
        QPushButton* m_collapseAllButton;
        QComboBox* m_versionCombo;
        GmodSearchBar* m_searchBar;
        QToolButton* m_legendButton;

    signals:
        void gmodChanged(const dnv::vista::sdk::Gmod& gmod);
    };
} // namespace qvistools::gui
