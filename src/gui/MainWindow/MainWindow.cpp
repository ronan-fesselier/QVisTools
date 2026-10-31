#include "MainWindow.h"

#include "qvistools/Version.h"

#include <QAction>
#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>

namespace qvistools::gui
{
    MainWindow::MainWindow(QWidget* parent)
        : QMainWindow{ parent }
    {
        setupStatusBar();
        setupFileMenu();
        setupHelpMenu();
    }

    void MainWindow::setupStatusBar()
    {
        statusBar()->showMessage(tr("Ready"));

        QObject::connect(statusBar(), &QStatusBar::messageChanged, statusBar(), [this](const QString& message) {
            if (message.isEmpty())
            {
                statusBar()->showMessage(tr("Ready"));
            }
        });
    }

    void MainWindow::setupFileMenu()
    {
        auto* fileMenu{ menuBar()->addMenu(tr("&File")) };

        auto* quitAction{ fileMenu->addAction(tr("&Quit")) };
        quitAction->setShortcut(QKeySequence::Quit);
        quitAction->setStatusTip(tr("Quit the application"));
        QObject::connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    }

    void MainWindow::setupHelpMenu()
    {
        auto* helpMenu{ menuBar()->addMenu(tr("&Help")) };

        auto* aboutAction{ helpMenu->addAction(tr("&About")) };
        aboutAction->setStatusTip(tr("About this application"));
        QObject::connect(aboutAction, &QAction::triggered, this, [this]() {
            auto* dialog{ new QDialog{ this } };
            dialog->setAttribute(Qt::WA_DeleteOnClose);
            dialog->setWindowTitle(tr("About"));
            dialog->setSizeGripEnabled(false);

            auto* titleLabel{ new QLabel{ dialog } };
            titleLabel->setTextFormat(Qt::RichText);
            titleLabel->setOpenExternalLinks(true);
            titleLabel->setAlignment(Qt::AlignHCenter);
            titleLabel->setText(tr("<h2><a href=\"https://github.com/ronan-fesselier/QVisTools\">QVisTools</a></h2>"));

            auto* infoLabel{ new QLabel{ dialog } };
            infoLabel->setTextFormat(Qt::RichText);
            infoLabel->setOpenExternalLinks(true);
            infoLabel->setText(tr("<table cellspacing=\"6\" cellpadding=\"2\">"
                                  "<tr><td align=\"right\">Version:</td><td><b>%1</b></td><td>&nbsp;(MIT)</td></tr>"
                                  "<tr><td align=\"right\"><a href=\"https://www.qt.io\">Qt</a>:</td><td><b>%2</b></td>"
                                  "<td>&nbsp;(LGPLv3)</td></tr>"
                                  "</table>")
                                   .arg(QString::fromUtf8(version()), QString::fromUtf8(qVersion())));

            auto* layout{ new QVBoxLayout{ dialog } };
            layout->setContentsMargins(24, 20, 24, 20);
            layout->setSpacing(12);
            layout->addWidget(titleLabel);
            layout->addWidget(infoLabel, 0, Qt::AlignHCenter);

            dialog->setFixedSize(dialog->sizeHint());
            dialog->move(window()->frameGeometry().center() - dialog->rect().center());

            dialog->exec();
        });
    }
} // namespace qvistools::gui
