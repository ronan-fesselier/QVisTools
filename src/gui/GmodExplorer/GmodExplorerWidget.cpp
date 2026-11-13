#include "GmodExplorerWidget.h"

#include "core/LocalIdBuilderUtils.h"
#include "detail/StringUtils.h"
#include "gui/GmodTree/GmodNodeDetailWidget.h"
#include "gui/GmodTree/GmodTreeWidget.h"
#include "gui/LocalId/LocalIdBuilderWidget.h"
#include "gui/Search/GmodSearchBar.h"
#include "gui/Shared/GmodChipRenderer.h"

#include <dnv/vista/sdk/core/GmodNode.h>
#include <dnv/vista/sdk/core/GmodPath.h>
#include <dnv/vista/sdk/core/LocalId.h>
#include <dnv/vista/sdk/core/ParsingErrors.h>
#include <dnv/vista/sdk/core/VIS.h>

#include <QApplication>
#include <QColor>
#include <QComboBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QSplitter>
#include <QStringList>
#include <QToolButton>
#include <QVBoxLayout>

namespace qvistools::gui
{
    GmodExplorerWidget::GmodExplorerWidget(const dnv::vista::sdk::Gmod& gmod, QWidget* parent)
        : QWidget{ parent },
          m_gmod{ &gmod },
          m_treeWidget{ new GmodTreeWidget{ gmod, this } },
          m_detailWidget{ new GmodNodeDetailWidget{ this } },
          m_localIdWidget{ new LocalIdBuilderWidget{ this } },
          m_collapseAllButton{ new QPushButton{ QIcon{ ":/icons/collapse-all.svg" }, QString{}, this } },
          m_versionCombo{ new QComboBox{ this } },
          m_searchBar{ new GmodSearchBar{ gmod, this } },
          m_localIdParseEdit{ new QLineEdit{ this } },
          m_legendButton{ new QToolButton{ this } },
          m_splitter{ new QSplitter{ Qt::Horizontal, this } },
          m_leftSplitter{ new QSplitter{ Qt::Vertical, this } }
    {
        m_collapseAllButton->setToolTip(tr("Collapse all"));
        m_versionCombo->setToolTip(tr("VIS version - Gmod structures differ between versions"));

        m_legendButton->setText(QStringLiteral("?"));
        m_legendButton->setAutoRaise(true);
        m_legendButton->setFocusPolicy(Qt::NoFocus);
        m_legendButton->setToolTip(chip::legendHtml());

        m_localIdParseEdit->setPlaceholderText(tr("Paste a LocalId and press Enter..."));
        m_localIdParseEdit->setClearButtonEnabled(true);

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
        hLayout->addWidget(m_searchBar, 1);
        hLayout->addWidget(m_localIdParseEdit, 1);
        hLayout->addWidget(m_collapseAllButton, 0, Qt::AlignTop);
        hLayout->addWidget(m_versionCombo, 0, Qt::AlignTop);
        hLayout->addWidget(m_legendButton, 0, Qt::AlignTop);

        auto* treePanel{ new QWidget{ this } };
        auto* treePanelLayout{ new QVBoxLayout{ treePanel } };
        treePanelLayout->setContentsMargins(0, 0, 0, 0);
        treePanelLayout->addLayout(hLayout);
        treePanelLayout->addWidget(m_treeWidget);

        m_leftSplitter->addWidget(treePanel);
        m_leftSplitter->addWidget(m_localIdWidget);
        m_leftSplitter->setStretchFactor(0, 3);
        m_leftSplitter->setStretchFactor(1, 1);
        m_leftSplitter->setCollapsible(0, false);

        m_splitter->addWidget(m_leftSplitter);
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
            m_treeWidget, &GmodTreeWidget::nodeSelectionChanged, this, [this](const dnv::vista::sdk::GmodNode* node) {
                if (m_navigatingToLocalId)
                {
                    return;
                }

                m_detailWidget->setNode(node, m_treeWidget->currentPath());
            });

        QObject::connect(m_localIdWidget, &LocalIdBuilderWidget::primaryItemRequested, this, [this]() {
            m_localIdWidget->setPath(m_treeWidget->currentPath());
        });

        QObject::connect(m_localIdWidget, &LocalIdBuilderWidget::secondaryItemRequested, this, [this]() {
            m_localIdWidget->setSecondaryItemPath(m_treeWidget->currentPath());
        });

        QObject::connect(
            m_localIdParseEdit, &QLineEdit::returnPressed, this, &GmodExplorerWidget::onLocalIdParseRequested);

        QObject::connect(
            m_localIdWidget, &LocalIdBuilderWidget::convertRequested, this, &GmodExplorerWidget::onConvertRequested);
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
        m_localIdWidget->setPath(std::nullopt);

        emit gmodChanged(*m_gmod);
    }

    void GmodExplorerWidget::onLocalIdParseRequested()
    {
        const auto text{ m_localIdParseEdit->text().trimmed() };
        if (text.isEmpty())
        {
            return;
        }

        dnv::vista::sdk::ParsingErrors errors{ {} };
        const auto localId{ dnv::vista::sdk::LocalId::fromString(text.toStdString(), errors) };
        if (!localId.has_value())
        {
            QPalette errorPalette{ m_localIdParseEdit->palette() };
            errorPalette.setColor(QPalette::Base, QColor{ 255, 204, 204 });
            m_localIdParseEdit->setPalette(errorPalette);

            QStringList lines;
            for (const auto& error : errors)
            {
                lines << detail::toQString(error.type) + QStringLiteral(": ") + detail::toQString(error.message);
            }
            m_localIdWidget->setParsingError(
                lines.isEmpty() ? tr("Failed to parse LocalId") : lines.join(QStringLiteral("\n")));
            return;
        }

        m_localIdParseEdit->setPalette(QApplication::palette());

        navigateToLocalId(*localId);

        m_localIdParseEdit->selectAll();
    }

    void GmodExplorerWidget::onConvertRequested(dnv::vista::sdk::VisVersion targetVersion)
    {
        const auto text{ m_localIdWidget->localIdText() };
        if (text.isEmpty())
        {
            return;
        }

        const auto localId{ dnv::vista::sdk::LocalId::fromString(text.toStdString()) };
        if (!localId.has_value())
        {
            return;
        }

        const auto converted{ core::convertLocalId(*localId, targetVersion) };
        if (!converted.has_value())
        {
            m_localIdWidget->setParsingError(tr("Conversion to the target VIS version failed"));
            return;
        }

        navigateToLocalId(*converted);
    }

    void GmodExplorerWidget::navigateToLocalId(const dnv::vista::sdk::LocalId& localId)
    {
        m_navigatingToLocalId = true;

        const auto version{ localId.version() };
        if (version != m_gmod->version())
        {
            const auto index{ m_versionCombo->findData(QVariant::fromValue(static_cast<int>(version))) };
            if (index >= 0)
            {
                m_versionCombo->setCurrentIndex(index);
            }
        }

        m_treeWidget->navigateTo(localId.primaryItem().withoutLocations().node());

        m_navigatingToLocalId = false;

        m_detailWidget->setNode(m_treeWidget->currentNode(), m_treeWidget->currentPath());
        m_localIdWidget->setLocalId(localId);
    }
} // namespace qvistools::gui
