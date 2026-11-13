#pragma once

#include <QWidget>

class QComboBox;
class QPushButton;
class QSplitter;
class QToolButton;

namespace dnv::vista::sdk
{
    class Gmod;
    class GmodNode;
} // namespace dnv::vista::sdk

namespace qvistools::gui
{
    class GmodNodeDetailWidget;
    class GmodSearchBar;
    class GmodTreeWidget;
    class LocalIdBuilderWidget;

    class GmodExplorerWidget final : public QWidget
    {
        Q_OBJECT

    public:
        explicit GmodExplorerWidget(const dnv::vista::sdk::Gmod& gmod, QWidget* parent = nullptr);

    private:
        void setupVersionCombo(const dnv::vista::sdk::Gmod& gmod);
        void setupLayout();
        void setupConnections();

        void onVersionChanged(int index);

        const dnv::vista::sdk::Gmod* m_gmod;

        GmodTreeWidget* m_treeWidget;
        GmodNodeDetailWidget* m_detailWidget;
        LocalIdBuilderWidget* m_localIdWidget;
        QPushButton* m_collapseAllButton;
        QComboBox* m_versionCombo;
        GmodSearchBar* m_searchBar;
        QToolButton* m_legendButton;
        QSplitter* m_splitter;
        QSplitter* m_leftSplitter;

    signals:
        void gmodChanged(const dnv::vista::sdk::Gmod& gmod);
    };
} // namespace qvistools::gui
