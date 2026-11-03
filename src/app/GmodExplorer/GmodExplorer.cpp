#include "GmodExplorer.h"

#include "app/detail/QStringUtils.h"
#include "app/Id/GmodPathIndividualizeDialog.h"
#include "app/shared/GmodChipRenderer.h"
#include "GmodSearchBar.h"
#include "GmodTreeItemDelegate.h"
#include "GmodTreeModel.h"

#include <viscore/gmod/NodeCategory.h>

#include <dnv/vista/sdk/core/VIS.h>

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPushButton>
#include <QStringList>
#include <QStyle>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

#include <array>

namespace qvistools
{
    namespace
    {
        QString legendHtml()
        {
            static constexpr std::array<viscore::gmod::NodeCategory, 10> categories{
                viscore::gmod::NodeCategory::Root,
                viscore::gmod::NodeCategory::AssetFunctionGroup,
                viscore::gmod::NodeCategory::ProductFunctionGroup,
                viscore::gmod::NodeCategory::AssetFunctionLeaf,
                viscore::gmod::NodeCategory::OtherLeaf,
                viscore::gmod::NodeCategory::FunctionComposition,
                viscore::gmod::NodeCategory::FunctionSelection,
                viscore::gmod::NodeCategory::ProductSelection,
                viscore::gmod::NodeCategory::ProductType,
                viscore::gmod::NodeCategory::Other
            };

            QString html{ QStringLiteral("<table cellpadding='2' cellspacing='0'>") };

            for (std::size_t i{ 0 }; i < categories.size();)
            {
                const auto color{ categoryColor(categories[i]) };

                QStringList names{ detail::toQString(viscore::gmod::categoryName(categories[i])) };
                std::size_t j{ i + 1 };
                while (j < categories.size() && categoryColor(categories[j]) == color)
                {
                    names << detail::toQString(viscore::gmod::categoryName(categories[j]));
                    ++j;
                }

                html += QStringLiteral(
                            "<tr>"
                            "<td><span style='background:%1'>&nbsp;&nbsp;&nbsp;</span></td>"
                            "<td>&nbsp;%2</td>"
                            "</tr>")
                            .arg(color.name(), names.join(QStringLiteral(" / ")));

                i = j;
            }

            return html + QStringLiteral("</table>");
        }
    } // namespace

    GmodExplorer::GmodExplorer(const dnv::vista::sdk::Gmod& gmod, QWidget* parent)
        : QGroupBox{ tr("Generic Product Model - VIS"), parent },
          m_gmod{ &gmod },
          m_model{ new GmodTreeModel{ *m_gmod, this } },
          m_treeView{ new QTreeView{ this } },
          m_collapseAllButton{ new QPushButton{ this } },
          m_versionCombo{ new QComboBox{ this } },
          m_searchBar{ new GmodSearchBar{ gmod, this } },
          m_legendButton{ new QToolButton{ this } }
    {
        m_collapseAllButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarShadeButton));
        m_collapseAllButton->setToolTip(tr("Collapse all"));
        m_collapseAllButton->setFixedWidth(m_collapseAllButton->sizeHint().width());

        m_legendButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation));
        m_legendButton->setAutoRaise(true);
        m_legendButton->setFocusPolicy(Qt::NoFocus);
        m_legendButton->setToolTip(legendHtml());

        setupTreeView();
        setupVersionCombo(gmod);
        setupLayout();

        setupConnections();

        expandRoot();
    }

    void GmodExplorer::setVisVersion(dnv::vista::sdk::VisVersion version)
    {
        m_versionCombo->setCurrentIndex(m_versionCombo->findData(QVariant::fromValue(static_cast<int>(version))));
    }

    void GmodExplorer::setupTreeView()
    {
        m_treeView->setModel(m_model);
        m_treeView->setUniformRowHeights(true);
        m_treeView->setItemDelegate(new GmodTreeItemDelegate{ m_treeView });
        m_treeView->setHeaderHidden(true);
        m_treeView->installEventFilter(this);
        m_treeView->viewport()->installEventFilter(this);
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

        m_versionCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        m_versionCombo->setFixedWidth(m_versionCombo->sizeHint().width());
    }

    void GmodExplorer::setupLayout()
    {
        auto* hLayout{ new QHBoxLayout };
        hLayout->addWidget(m_searchBar);
        hLayout->addWidget(m_collapseAllButton, 0, Qt::AlignTop);
        hLayout->addWidget(m_versionCombo, 0, Qt::AlignTop);
        hLayout->addWidget(m_legendButton, 0, Qt::AlignTop);

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

        QObject::connect(m_searchBar, &GmodSearchBar::matchActivated, this, &GmodExplorer::onSearchMatchActivated);
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

        m_searchBar->setGmod(*m_gmod);

        emit gmodChanged(*m_gmod);
    }

    void GmodExplorer::onRootCollapsed(const QModelIndex& index)
    {
        if (!index.parent().isValid())
        {
            m_treeView->setExpanded(index, true);
        }
    }

    void GmodExplorer::onSearchMatchActivated(const viscore::gmod::SearchIndex::Match& match)
    {
        const auto targetIndex{ m_model->indexForMatch(match) };
        if (!targetIndex.isValid())
        {
            return;
        }

        for (auto parentIndex{ targetIndex.parent() }; parentIndex.isValid(); parentIndex = parentIndex.parent())
        {
            m_treeView->expand(parentIndex);
        }

        m_treeView->setCurrentIndex(targetIndex);
        m_treeView->scrollTo(targetIndex, QAbstractItemView::PositionAtCenter);
    }

    bool GmodExplorer::onTreeViewMousePress(QMouseEvent* event)
    {
        if (event->button() != Qt::LeftButton)
        {
            return false;
        }

        const auto index{ m_treeView->indexAt(event->pos()) };
        if (!index.isValid())
        {
            return false;
        }

        const auto segments{ GmodTreeItemDelegate::buildSegments(index) };
        if (segments.empty())
        {
            return false;
        }

        const auto rowRect{ m_treeView->visualRect(index) };
        const QFontMetrics metrics{ m_treeView->font() };
        const auto hit{ hitTest(metrics, GmodTreeItemDelegate::chipTopLeft(rowRect), segments, event->pos()) };
        if (!hit.has_value() || segments[*hit].icon != ChipIcon::Link)
        {
            return false;
        }

        const auto path{ m_model->pathForIndex(index) };
        if (!path.has_value())
        {
            return false;
        }

        auto* menu{ new QMenu{ m_treeView } };
        menu->setAttribute(Qt::WA_DeleteOnClose);
        auto* primaryAction{ menu->addAction(tr("Map to Primary")) };
        auto* secondaryAction{ menu->addAction(tr("Map to Secondary")) };

        const auto mapWithIndividualization{ [this, path](MapTarget target) {
            if (!path->isIndividualizable())
            {
                emit nodeMapped(*path, target);
                return;
            }

            GmodPathIndividualizeDialog dialog{ *path, std::nullopt, m_treeView };
            if (dialog.exec() == QDialog::Accepted)
            {
                emit nodeMapped(dialog.result(), target);
            }
        } };

        QObject::connect(primaryAction, &QAction::triggered, this, [mapWithIndividualization]() {
            mapWithIndividualization(MapTarget::Primary);
        });
        QObject::connect(secondaryAction, &QAction::triggered, this, [mapWithIndividualization]() {
            mapWithIndividualization(MapTarget::Secondary);
        });

        menu->popup(event->globalPosition().toPoint());
        return true;
    }

    bool GmodExplorer::eventFilter(QObject* watched, QEvent* event)
    {
        if (watched == m_treeView && event->type() == QEvent::KeyPress)
        {
            auto* keyEvent{ static_cast<QKeyEvent*>(event) };

            if (keyEvent->key() == Qt::Key_Asterisk)
            {
                // Block QTreeView's default expand-all shortcut: the Gmod DAG is too large to expand fully
                return true;
            }
        }

        if (watched == m_treeView->viewport() && event->type() == QEvent::MouseButtonPress)
        {
            if (onTreeViewMousePress(static_cast<QMouseEvent*>(event)))
            {
                return true;
            }
        }

        return QWidget::eventFilter(watched, event);
    }
} // namespace qvistools
