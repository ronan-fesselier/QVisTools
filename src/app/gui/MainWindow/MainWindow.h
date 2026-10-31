#pragma once

#include <QMainWindow>

namespace qvistools::gui
{
    class MainWindow final : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent = nullptr);

    private:
        void setupStatusBar();
        void setupFileMenu();
        void setupHelpMenu();
    };
} // namespace qvistools::gui
