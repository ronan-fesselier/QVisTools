#include "App.h"

#include "gui/MainWindow/MainWindow.h"
#include "qvistools/Version.h"

#include <QApplication>

namespace qvistools
{
    int App::run(int argc, char* argv[])
    {
        QApplication app{ argc, argv };
        QApplication::setApplicationName(QStringLiteral("QVisTools"));
        QApplication::setApplicationVersion(QString::fromUtf8(version()));

        gui::MainWindow window;
        window.setWindowTitle("QVisTools");
        window.setMinimumSize(800, 600);

        window.show();

        return app.exec();
    }
} // namespace qvistools
