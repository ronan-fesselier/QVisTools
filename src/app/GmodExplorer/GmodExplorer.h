#pragma once

#include <viscore/gmod/SearchIndex.h>

#include <dnv/vista/sdk/core/VisVersions.h>

#include <QGroupBox>
#include <QModelIndex>

#include <optional>

class QComboBox;
class QEvent;
class QMouseEvent;
class QPushButton;
class QToolButton;
class QTreeView;

namespace dnv::vista::sdk
{
    class Gmod;
    class GmodPath;
} // namespace dnv::vista::sdk

namespace qvistools
{
    class GmodSearchBar;
    class GmodTreeModel;

    class GmodExplorer final : public QGroupBox
    {
        Q_OBJECT

    public:
        enum class MapTarget
        {
            Primary,
            Secondary
        };

        explicit GmodExplorer(const dnv::vista::sdk::Gmod& gmod, QWidget* parent = nullptr);

        void setVisVersion(dnv::vista::sdk::VisVersion version);

    private:
        void setupTreeView();
        void setupVersionCombo(const dnv::vista::sdk::Gmod& gmod);
        void setupLayout();
        void setupConnections();

        void expandRoot();
        void collapseAll();

        void onVersionChanged(int index);
        void onRootCollapsed(const QModelIndex& index);
        void onSearchMatchActivated(const viscore::gmod::SearchIndex::Match& match);
        bool onTreeViewMousePress(QMouseEvent* event);

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
        void nodeMapped(const dnv::vista::sdk::GmodPath& path, GmodExplorer::MapTarget target);
    };
} // namespace qvistools
