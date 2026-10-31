#include "app/detail/QStringUtils.h"
#include "app/MainWindow/MainWindow.h"

#include "qvistools/Version.h"

#include <dnv/vista/sdk/core/VIS.h>

#include <QApplication>
#include <QIcon>
#include <QMessageBox>

#include <exception>

int main(int argc, char* argv[])
{
    QApplication app{ argc, argv };
    QApplication::setApplicationName(QStringLiteral("QVisTools"));
    QApplication::setApplicationVersion(QString::fromUtf8(qvistools::version()));
    QApplication::setWindowIcon(QIcon{ QStringLiteral(":/icons/ship.png") });

    try
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& gmod{ vis.gmod(vis.latest()) };

        qvistools::MainWindow window{ gmod };
        window.setWindowTitle(QStringLiteral("QVisTools"));

        window.show();

        return app.exec();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(
            nullptr,
            QCoreApplication::translate("QVisTools", "Startup error"),
            qvistools::detail::toQString(e.what() != nullptr ? e.what() : "Unknown error"));

        return 1;
    }
}
