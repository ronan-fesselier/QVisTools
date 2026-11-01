#pragma once

#include <QWidget>

class QComboBox;
class QEvent;
class QPushButton;
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

        const dnv::vista::sdk::Gmod* m_gmod;

        GmodTreeModel* m_model;
        QTreeView* m_treeView;
        QPushButton* m_collapseAllButton;
        QComboBox* m_versionCombo;
        GmodSearchBar* m_searchBar;

    signals:
        void gmodChanged(const dnv::vista::sdk::Gmod& gmod);
    };
} // namespace qvistools::gui
