#include "GmodExplorerWidget.h"

#include "detail/StringUtils.h"
#include "GmodChipRenderer.h"
#include "GmodNodeDetailWidget.h"
#include "GmodSearchBar.h"
#include "GmodTreeWidget.h"

#include <dnv/vista/sdk/core/GmodNode.h>
#include <dnv/vista/sdk/core/VIS.h>

#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QSplitter>
#include <QToolButton>
#include <QVBoxLayout>

namespace qvistools::gui
{
    GmodExplorerWidget::GmodExplorerWidget(const dnv::vista::sdk::Gmod& gmod, QWidget* parent)
        : QWidget{ parent },
          m_gmod{ &gmod },
          m_treeWidget{ new GmodTreeWidget{ gmod, this } },
          m_detailWidget{ new GmodNodeDetailWidget{ this } },
          m_collapseAllButton{ new QPushButton{ QIcon{ ":/icons/collapse-all.svg" }, QString{}, this } },
          m_versionCombo{ new QComboBox{ this } },
          m_searchBar{ new GmodSearchBar{ gmod, this } },
          m_legendButton{ new QToolButton{ this } },
          m_splitter{ new QSplitter{ Qt::Horizontal, this } }
    {
        m_collapseAllButton->setToolTip(tr("Collapse all"));
        m_versionCombo->setToolTip(tr("VIS version - Gmod structures differ between versions"));

        m_legendButton->setText(QStringLiteral("?"));
        m_legendButton->setAutoRaise(true);
        m_legendButton->setFocusPolicy(Qt::NoFocus);
        m_legendButton->setToolTip(chip::legendHtml());

        setupVersionCombo(gmod);
        setupLayout();

        setupConnections();
    }

    void GmodExplorerWidget::setupVersionCombo(const dnv::vista::sdk::Gmod& gmod)
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

    void GmodExplorerWidget::setupLayout()
    {
        auto* hLayout{ new QHBoxLayout };
        hLayout->addWidget(m_searchBar);
        hLayout->addWidget(m_collapseAllButton, 0, Qt::AlignTop);
        hLayout->addWidget(m_versionCombo, 0, Qt::AlignTop);
        hLayout->addWidget(m_legendButton, 0, Qt::AlignTop);

        auto* treePanel{ new QWidget{ this } };
        auto* treePanelLayout{ new QVBoxLayout{ treePanel } };
        treePanelLayout->setContentsMargins(0, 0, 0, 0);
        treePanelLayout->addLayout(hLayout);
        treePanelLayout->addWidget(m_treeWidget);

        m_splitter->addWidget(treePanel);
        m_splitter->addWidget(m_detailWidget);
        m_splitter->setStretchFactor(0, 3);
        m_splitter->setStretchFactor(1, 1);
        m_splitter->setCollapsible(0, false);

        auto* layout{ new QVBoxLayout{ this } };
        layout->addWidget(m_splitter);
    }

    void GmodExplorerWidget::setupConnections()
    {
        QObject::connect(m_collapseAllButton, &QPushButton::clicked, m_treeWidget, &GmodTreeWidget::collapseAll);

        QObject::connect(
            m_versionCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &GmodExplorerWidget::onVersionChanged);

        QObject::connect(m_searchBar, &GmodSearchBar::nodeActivated, m_treeWidget, &GmodTreeWidget::navigateTo);

        QObject::connect(
            m_treeWidget, &GmodTreeWidget::nodeSelectionChanged, m_detailWidget, &GmodNodeDetailWidget::setNode);
    }

    void GmodExplorerWidget::onVersionChanged(int index)
    {
        if (index < 0)
        {
            return;
        }

        const auto version{ static_cast<dnv::vista::sdk::VisVersion>(m_versionCombo->itemData(index).toInt()) };

        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        m_gmod = &vis.gmod(version);

        m_treeWidget->setGmod(*m_gmod);
        m_searchBar->setGmod(*m_gmod);

        emit gmodChanged(*m_gmod);
    }
} // namespace qvistools::gui
