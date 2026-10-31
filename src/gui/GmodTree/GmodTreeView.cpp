#include "GmodTreeView.h"

#include "detail/StringUtils.h"
#include "GmodTreeItemDelegate.h"
#include "GmodTreeModel.h"

#include <dnv/vista/sdk/core/VIS.h>

#include <QComboBox>
#include <QEvent>
#include <QIcon>
#include <QKeyEvent>
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>

namespace qvistools::gui
{
    GmodTreeView::GmodTreeView(const dnv::vista::sdk::Gmod& gmod, QWidget* parent)
        : QWidget{ parent },
          m_gmod{ &gmod },
          m_model{ new GmodTreeModel{ *m_gmod, this } },
          m_treeView{ new QTreeView{ this } },
          m_collapseAllButton{ new QPushButton{ QIcon{ ":/icons/collapse-all.svg" }, QString{}, this } },
          m_versionCombo{ new QComboBox{ this } }
    {
        m_collapseAllButton->setToolTip(tr("Collapse all"));

        setupTreeView();
        setupVersionCombo(gmod);
        setupLayout();

        setupConnections();

        expandRoot();
    }

    void GmodTreeView::setupTreeView()
    {
        m_treeView->setModel(m_model);
        m_treeView->setHeaderHidden(true);
        m_treeView->installEventFilter(this);
        m_treeView->setItemDelegate(new GmodTreeItemDelegate{ m_treeView });
    }

    void GmodTreeView::setupVersionCombo(const dnv::vista::sdk::Gmod& gmod)
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        for (const auto version : vis.versions())
        {
            const auto versionStr{ dnv::vista::sdk::VisVersions::toString(version) };
            m_versionCombo->addItem(detail::toQString(versionStr), QVariant::fromValue(static_cast<int>(version)));
        }
        m_versionCombo->setCurrentIndex(
            m_versionCombo->findData(QVariant::fromValue(static_cast<int>(gmod.version()))));
    }

    void GmodTreeView::setupLayout()
    {
        auto* hLayout{ new QHBoxLayout };
        hLayout->addWidget(m_collapseAllButton);
        hLayout->addWidget(m_versionCombo);

        auto* layout{ new QVBoxLayout{ this } };
        layout->addLayout(hLayout);
        layout->addWidget(m_treeView);
    }

    void GmodTreeView::setupConnections()
    {
        QObject::connect(m_collapseAllButton, &QPushButton::clicked, this, &GmodTreeView::collapseAll);

        QObject::connect(
            m_versionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &GmodTreeView::onVersionChanged);
        QObject::connect(m_treeView, &QTreeView::collapsed, this, &GmodTreeView::onRootCollapsed);
    }

    void GmodTreeView::expandRoot()
    {
        m_treeView->setExpanded(m_model->index(0, 0), true);
    }

    void GmodTreeView::collapseAll()
    {
        m_treeView->collapseAll();
        expandRoot();
    }

    bool GmodTreeView::eventFilter(QObject* watched, QEvent* event)
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

    void GmodTreeView::onVersionChanged(int index)
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

    void GmodTreeView::onRootCollapsed(const QModelIndex& index)
    {
        if (!index.parent().isValid())
        {
            m_treeView->setExpanded(index, true);
        }
    }
} // namespace qvistools::gui
