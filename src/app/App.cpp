#include "App.h"

#include "gui/MainWindow/MainWindow.h"
#include "qvistools/Version.h"

#include <dnv/vista/sdk/core/VIS.h>

#include <QApplication>
#include <QMessageBox>

#include <exception>

namespace qvistools
{
    int App::run(int argc, char* argv[])
    {
        QApplication app{ argc, argv };
        QApplication::setApplicationName(QStringLiteral("QVisTools"));
        QApplication::setApplicationVersion(QString::fromUtf8(version()));

        try
        {
            const auto& vis{ dnv::vista::sdk::VIS::instance() };
            const auto& gmod{ vis.gmod(vis.latest()) };

            gui::MainWindow window{ gmod };
            window.setWindowTitle("QVisTools");
            window.setMinimumSize(800, 600);

            window.show();

            return app.exec();
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(nullptr, QObject::tr("Startup error"), QString::fromUtf8(e.what()));

            return 1;
        }
    }
} // namespace qvistools
