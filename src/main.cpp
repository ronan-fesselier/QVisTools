#include <QApplication>
#include <QMainWindow>

int main(int argc, char* argv[])
{
    QApplication app{ argc, argv };
    QApplication::setApplicationName(QStringLiteral("QVisTools"));

    QMainWindow window;
    window.setWindowTitle(QStringLiteral("QVisTools"));
    window.setMinimumSize(800, 600);
    window.show();

    return app.exec();
}
