#include "MainWindow.h"

#include "detail/StringUtils.h"
#include "gui/GmodTree/GmodTreeView.h"

#include <dnv/vista/sdk/core/Gmod.h>
#include <dnv/vista/sdk/Version.h>

#include <QAction>
#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>

#include <string>

namespace qvistools::gui
{
    MainWindow::MainWindow(const dnv::vista::sdk::Gmod& gmod, QWidget* parent)
        : QMainWindow{ parent },
          m_treeView{ new GmodTreeView{ gmod, this } }
    {
        setupStatusBar(gmod);
        setupFileMenu();
        setupHelpMenu();

        setCentralWidget(m_treeView);
    }

    void MainWindow::setupStatusBar(const dnv::vista::sdk::Gmod& gmod)
    {
        statusBar()->showMessage(tr("Ready"));

        QObject::connect(statusBar(), &QStatusBar::messageChanged, statusBar(), [this](const QString& message) {
            if (message.isEmpty())
            {
                statusBar()->showMessage(tr("Ready"));
            }
        });

        auto* versionLabel{ new QLabel{ this } };
        statusBar()->addPermanentWidget(versionLabel);

        const auto updateVersionLabel{ [versionLabel](const dnv::vista::sdk::Gmod& g) {
            const auto version{ dnv::vista::sdk::VisVersions::toString(g.version()) };
            const auto nodeCount{ std::distance(g.begin(), g.end()) };
            versionLabel->setText(tr("Gmod %1 (%2 nodes)").arg(detail::toQString(version)).arg(nodeCount));
        } };
        updateVersionLabel(gmod);
        QObject::connect(m_treeView, &GmodTreeView::gmodChanged, this, updateVersionLabel);
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
            infoLabel->setText(
                tr("<table cellspacing=\"6\" cellpadding=\"2\">"
                   "<tr><td align=\"right\">Version:</td><td><b>%1</b></td><td>&nbsp;(MIT)</td></tr>"
                   "<tr><td align=\"right\"><a "
                   "href=\"https://github.com/ronan-fesselier/vista-sdk/tree/feature/cpp/cpp\">DNV Vista "
                   "SDK C++</a>:</td><td><b>%2</b></td><td>&nbsp;(MIT)</td></tr>"
                   "<tr><td align=\"right\"><a href=\"https://www.qt.io\">Qt</a>:</td><td><b>%3</b></td>"
                   "<td>&nbsp;(LGPLv3)</td></tr>"
                   "</table>")
                    .arg(
                        QApplication::applicationVersion(),
                        detail::toQString(dnv::vista::sdk::version()),
                        detail::toQString(qVersion())));

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
