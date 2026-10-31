#include "app/gui/MainWindow/MainWindow.h"

#include "qvistools/Version.h"

#include <QApplication>

using namespace qvistools;

int main(int argc, char* argv[])
{
    QApplication app{ argc, argv };
    QApplication::setApplicationName(QStringLiteral("QVisTools"));
    QApplication::setApplicationVersion(QString::fromUtf8(qvistools::version()));

    gui::MainWindow window;
    window.setWindowTitle(QStringLiteral("QVisTools"));
    window.setMinimumSize(800, 600);

    window.show();

    return app.exec();
}
