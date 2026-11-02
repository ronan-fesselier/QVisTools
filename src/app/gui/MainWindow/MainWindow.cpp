#include "MainWindow.h"

#include "qvistools/Version.h"

#include "app/gui/detail/QStringUtils.h"
#include "app/gui/GmodExplorer/GmodExplorer.h"
#include "app/gui/LocalId/LocalIdBuilderWidget.h"

#include <dnv/vista/sdk/core/Gmod.h>
#include <dnv/vista/sdk/core/GmodPath.h>
#include <dnv/vista/sdk/Version.h>

#include <QAction>
#include <QApplication>
#include <QDialog>
#include <QFrame>
#include <QLabel>
#include <QMenuBar>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>

#include <iterator>

namespace qvistools::gui
{
    MainWindow::MainWindow(const dnv::vista::sdk::Gmod& gmod, QWidget* parent)
        : QMainWindow{ parent },
          m_localIdBuilder{ new LocalIdBuilderWidget{ gmod.version(), this } },
          m_explorer{ new GmodExplorer{ gmod, this } }
    {
        setupStatusBar(gmod);
        setupFileMenu();
        setupHelpMenu();
        setupLocalIdConnections();

        auto* splitter{ new QSplitter{ Qt::Horizontal, this } };
        splitter->setChildrenCollapsible(false);
        splitter->addWidget(m_localIdBuilder);
        splitter->addWidget(m_explorer);
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 1);

        auto* centralWidget{ new QWidget{ this } };
        auto* layout{ new QVBoxLayout{ centralWidget } };
        layout->setContentsMargins(8, 8, 8, 8);
        layout->addWidget(splitter);

        setCentralWidget(centralWidget);
    }

    void MainWindow::setupStatusBar(const dnv::vista::sdk::Gmod& gmod)
    {
        statusBar()->showMessage(tr("Ready"));

        QObject::connect(statusBar(), &QStatusBar::messageChanged, this, [this](const QString& message) {
            if (message.isEmpty())
            {
                statusBar()->showMessage(tr("Ready"));
            }
        });

        auto* versionLabel{ new QLabel{ this } };
        versionLabel->setFrameStyle(QFrame::NoFrame);
        statusBar()->addPermanentWidget(versionLabel);

        const auto updateVersionLabel{ [versionLabel](const dnv::vista::sdk::Gmod& g) {
            const auto version{ dnv::vista::sdk::VisVersions::toString(g.version()) };
            const auto nodeCount{ std::distance(g.begin(), g.end()) };
            versionLabel->setText(tr("Gmod %1 (%2 nodes)").arg(detail::toQString(version)).arg(nodeCount));
        } };
        updateVersionLabel(gmod);
        QObject::connect(m_explorer, &GmodExplorer::gmodChanged, this, updateVersionLabel);
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
            QDialog dialog{ this };
            dialog.setWindowTitle(tr("About"));
            dialog.setSizeGripEnabled(false);

            auto* titleLabel{ new QLabel{ &dialog } };
            titleLabel->setTextFormat(Qt::RichText);
            titleLabel->setOpenExternalLinks(true);
            titleLabel->setAlignment(Qt::AlignHCenter);
            titleLabel->setText(tr("<h2><a href=\"https://github.com/ronan-fesselier/QVisTools\">QVisTools</a></h2>"));

            auto* infoLabel{ new QLabel{ &dialog } };
            infoLabel->setTextFormat(Qt::RichText);
            infoLabel->setOpenExternalLinks(true);
            infoLabel->setText(
                tr("<table cellspacing=\"6\" cellpadding=\"2\">"
                   "<tr><td align=\"right\">Version:</td><td><b>%1</b></td><td>&nbsp;(MIT)</td></tr>"
                   "<tr><td align=\"right\"><a "
                   "href=\"https://github.com/ronan-fesselier/vista-sdk/tree/feature/cpp/cpp\">DNV Vista "
                   "SDK</a>:</td><td><b>%2</b></td><td>&nbsp;(MIT)</td></tr>"
                   "<tr><td align=\"right\"><a href=\"https://www.qt.io\">Qt</a>:</td><td><b>%3</b></td>"
                   "<td>&nbsp;(LGPLv3)</td></tr>"
                   "<tr><td align=\"right\">Icons:</td><td colspan=\"2\"><a "
                   "href=\"https://icons8.com\">Icons8</a></td></tr>"
                   "</table>")
                    .arg(
                        QApplication::applicationVersion(),
                        detail::toQString(dnv::vista::sdk::version()),
                        detail::toQString(qVersion())));

            auto* layout{ new QVBoxLayout{ &dialog } };
            layout->setContentsMargins(24, 20, 24, 20);
            layout->setSpacing(12);
            layout->addWidget(titleLabel);
            layout->addWidget(infoLabel, 0, Qt::AlignHCenter);

            dialog.setFixedSize(dialog.sizeHint());
            dialog.move(window()->frameGeometry().center() - dialog.rect().center());

            dialog.exec();
        });
    }

    void MainWindow::setupLocalIdConnections()
    {
        QObject::connect(
            m_explorer,
            &GmodExplorer::nodeMapped,
            this,
            [this](const dnv::vista::sdk::GmodPath& path, GmodExplorer::MapTarget target) {
                switch (target)
                {
                    case GmodExplorer::MapTarget::Primary:
                        m_localIdBuilder->setPrimaryItem(path);
                        break;
                    case GmodExplorer::MapTarget::Secondary:
                        m_localIdBuilder->setSecondaryItem(path);
                        break;
                }
            });

        QObject::connect(m_explorer, &GmodExplorer::gmodChanged, this, [this](const dnv::vista::sdk::Gmod& g) {
            m_localIdBuilder->setVisVersion(g.version());
        });

        QObject::connect(m_localIdBuilder, &LocalIdBuilderWidget::primaryItemUnmapRequested, this, [this]() {
            m_localIdBuilder->setPrimaryItem(std::nullopt);
        });
        QObject::connect(m_localIdBuilder, &LocalIdBuilderWidget::secondaryItemUnmapRequested, this, [this]() {
            m_localIdBuilder->setSecondaryItem(std::nullopt);
        });
    }
} // namespace qvistools::gui
