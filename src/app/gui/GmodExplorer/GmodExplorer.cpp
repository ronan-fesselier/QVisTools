#include "GmodExplorer.h"

#include "app/gui/detail/QStringUtils.h"
#include "app/gui/GmodTree/GmodTreeModel.h"

#include <dnv/vista/sdk/core/VIS.h>

#include <QApplication>
#include <QComboBox>
#include <QEvent>
#include <QKeyEvent>
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>

namespace qvistools::gui
{
    GmodExplorer::GmodExplorer(const dnv::vista::sdk::Gmod& gmod, QWidget* parent)
        : QGroupBox{ tr("Generic Product Model - VIS"), parent },
          m_gmod{ &gmod },
          m_model{ new GmodTreeModel{ *m_gmod, this } },
          m_treeView{ new QTreeView{ this } },
          m_collapseAllButton{ new QPushButton{ this } },
          m_versionCombo{ new QComboBox{ this } }
    {
        m_collapseAllButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarShadeButton));
        m_collapseAllButton->setToolTip(tr("Collapse all"));

        setupTreeView();
        setupVersionCombo(gmod);
        setupLayout();

        setupConnections();

        expandRoot();
    }

    void GmodExplorer::setupTreeView()
    {
        m_treeView->setModel(m_model);
        m_treeView->setUniformRowHeights(true);
        m_treeView->setHeaderHidden(true);
        m_treeView->installEventFilter(this);
    }

    void GmodExplorer::setupVersionCombo(const dnv::vista::sdk::Gmod& gmod)
    {
        m_versionCombo->setToolTip(tr("VIS version"));

        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        for (const auto version : vis.versions())
        {
            const auto versionStr{ dnv::vista::sdk::VisVersions::toString(version) };
            m_versionCombo->addItem(detail::toQString(versionStr), QVariant::fromValue(static_cast<int>(version)));
        }
        m_versionCombo->setCurrentIndex(
            m_versionCombo->findData(QVariant::fromValue(static_cast<int>(gmod.version()))));
    }

    void GmodExplorer::setupLayout()
    {
        auto* hLayout{ new QHBoxLayout };
        hLayout->addWidget(m_collapseAllButton);
        hLayout->addWidget(m_versionCombo);

        auto* layout{ new QVBoxLayout{ this } };
        layout->addLayout(hLayout);
        layout->addWidget(m_treeView);
    }

    void GmodExplorer::setupConnections()
    {
        QObject::connect(m_collapseAllButton, &QPushButton::clicked, this, &GmodExplorer::collapseAll);

        QObject::connect(
            m_versionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &GmodExplorer::onVersionChanged);

        QObject::connect(m_treeView, &QTreeView::collapsed, this, &GmodExplorer::onRootCollapsed);
    }

    void GmodExplorer::expandRoot()
    {
        m_treeView->setExpanded(m_model->index(0, 0), true);
    }

    void GmodExplorer::collapseAll()
    {
        m_treeView->collapseAll();
        expandRoot();
    }

    void GmodExplorer::onVersionChanged(int index)
    {
        if (index < 0)
        {
            return;
        }

        const auto version{ static_cast<dnv::vista::sdk::VisVersion>(m_versionCombo->itemData(index).toInt()) };

        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        m_gmod = &vis.gmod(version);

        auto* newModel{ new GmodTreeModel{ *m_gmod, this } };
        m_treeView->setModel(newModel);
        m_treeView->collapseAll();

        m_model->deleteLater();
        m_model = newModel;

        expandRoot();

        emit gmodChanged(*m_gmod);
    }

    void GmodExplorer::onRootCollapsed(const QModelIndex& index)
    {
        if (!index.parent().isValid())
        {
            m_treeView->setExpanded(index, true);
        }
    }

    bool GmodExplorer::eventFilter(QObject* watched, QEvent* event)
    {
        if (watched == m_treeView && event->type() == QEvent::KeyPress)
        {
            auto* keyEvent{ static_cast<QKeyEvent*>(event) };

            if (keyEvent->key() == Qt::Key_Asterisk)
            {
                // Block QTreeView's default expand-all shortcut: the Gmod DAG is too large to expand fully.
                return true;
            }
        }

        return QWidget::eventFilter(watched, event);
    }
} // namespace qvistools::gui
