#pragma once

#include "core/vis/gmod/SearchIndex.h"

#include <QGroupBox>
#include <QModelIndex>

class QComboBox;
class QEvent;
class QPushButton;
class QToolButton;
class QTreeView;

namespace dnv::vista::sdk
{
    class Gmod;
} // namespace dnv::vista::sdk

namespace qvistools::gui
{
    class GmodSearchBar;
    class GmodTreeModel;

    class GmodExplorer final : public QGroupBox
    {
        Q_OBJECT

    public:
        explicit GmodExplorer(const dnv::vista::sdk::Gmod& gmod, QWidget* parent = nullptr);

    private:
        void setupTreeView();
        void setupVersionCombo(const dnv::vista::sdk::Gmod& gmod);
        void setupLayout();
        void setupConnections();

        void expandRoot();
        void collapseAll();

        void onVersionChanged(int index);
        void onRootCollapsed(const QModelIndex& index);
        void onSearchMatchActivated(const core::vis::SearchIndex::Match& match);

        const dnv::vista::sdk::Gmod* m_gmod;

        GmodTreeModel* m_model;
        QTreeView* m_treeView;
        QPushButton* m_collapseAllButton;
        QComboBox* m_versionCombo;
        GmodSearchBar* m_searchBar;
        QToolButton* m_legendButton;

    protected:
        virtual bool eventFilter(QObject* watched, QEvent* event) override;

    signals:
        void gmodChanged(const dnv::vista::sdk::Gmod& gmod);
    };
} // namespace qvistools::gui
