#pragma once

#include <QMainWindow>

namespace dnv::vista::sdk
{
    class Gmod;
} // namespace dnv::vista::sdk

namespace qvistools::gui
{
    class GmodExplorerWidget;

    class MainWindow final : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(const dnv::vista::sdk::Gmod& gmod, QWidget* parent = nullptr);

    private:
        void setupStatusBar(const dnv::vista::sdk::Gmod& gmod);
        void setupFileMenu();
        void setupHelpMenu();

        GmodExplorerWidget* m_explorerWidget;
    };
} // namespace qvistools::gui
