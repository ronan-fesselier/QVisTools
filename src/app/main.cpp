#include "app/MainWindow/MainWindow.h"

#include "qvistools/Version.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char* argv[])
{
    QApplication app{ argc, argv };
    QApplication::setApplicationName(QStringLiteral("QVisTools"));
    QApplication::setApplicationVersion(QString::fromUtf8(qvistools::version()));
    QApplication::setWindowIcon(QIcon{ QStringLiteral(":/icons/ship.png") });

    qvistools::MainWindow window;
    window.setWindowTitle(QStringLiteral("QVisTools"));

    window.show();

    return app.exec();
}
