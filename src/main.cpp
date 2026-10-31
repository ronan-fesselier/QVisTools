#include <QApplication>
#include <QMainWindow>

int main(int argc, char* argv[])
{
    QApplication app{ argc, argv };
    QApplication::setApplicationName(QStringLiteral("QVisTools"));

    QMainWindow window;
    window.setWindowTitle(QStringLiteral("QVisTools"));
    window.show();

    return app.exec();
}
