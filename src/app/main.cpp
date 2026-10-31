#include "app/gui/detail/QStringUtils.h"
#include "app/gui/MainWindow/MainWindow.h"

#include "qvistools/Version.h"

#include <dnv/vista/sdk/core/VIS.h>

#include <QApplication>
#include <QMessageBox>

#include <exception>

using namespace qvistools;

int main(int argc, char* argv[])
{
    QApplication app{ argc, argv };
    QApplication::setApplicationName(QStringLiteral("QVisTools"));
    QApplication::setApplicationVersion(detail::toQString(qvistools::version()));

    try
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        gui::MainWindow window{ gmod };
        window.setWindowTitle(QStringLiteral("QVisTools"));
        window.setMinimumSize(800, 600);

        window.show();

        return app.exec();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(
            nullptr,
            QCoreApplication::translate("QVisTools", "Startup error"),
            detail::toQString(e.what() != nullptr ? e.what() : "Unknown error"));

        return 1;
    }
}
