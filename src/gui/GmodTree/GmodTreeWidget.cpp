#include "GmodTreeWidget.h"

#include "gui/SdkQtTypes.h"
#include "GmodTreeItemDelegate.h"
#include "GmodTreeModel.h"

#include <dnv/vista/sdk/core/GmodNode.h>

#include <QEvent>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QTreeView>
#include <QVBoxLayout>

namespace qvistools::gui
{
    GmodTreeWidget::GmodTreeWidget(const dnv::vista::sdk::Gmod& gmod, QWidget* parent)
        : QWidget{ parent },
          m_model{ new GmodTreeModel{ gmod, this } },
          m_treeView{ new QTreeView{ this } }
    {
        setupTreeView();
        setupLayout();
        setupConnections();

        expandRoot();
    }

    void GmodTreeWidget::setGmod(const dnv::vista::sdk::Gmod& gmod)
    {
        auto* newModel{ new GmodTreeModel{ gmod, this } };
        m_treeView->setModel(newModel);
        m_treeView->collapseAll();

        m_model->deleteLater();
        m_model = newModel;

        expandRoot();
    }

    const dnv::vista::sdk::GmodNode* GmodTreeWidget::currentNode() const
    {
        return m_treeView->currentIndex().data(GmodTreeModel::NodeRole).value<const dnv::vista::sdk::GmodNode*>();
    }

    std::optional<dnv::vista::sdk::GmodPath> GmodTreeWidget::currentPath() const
    {
        if (const auto* node = currentNode(); node != nullptr)
        {
            return m_model->pathForNode(*node);
        }
        return std::nullopt;
    }

    void GmodTreeWidget::navigateTo(const dnv::vista::sdk::GmodNode& node)
    {
        const auto index{ m_model->indexForNode(node) };
        if (!index.isValid())
        {
            return;
        }

        for (auto ancestor{ index.parent() }; ancestor.isValid(); ancestor = ancestor.parent())
        {
            m_treeView->setExpanded(ancestor, true);
        }

        m_treeView->setCurrentIndex(index);
        m_treeView->scrollTo(index, QAbstractItemView::PositionAtCenter);
        m_treeView->setFocus();
    }

    void GmodTreeWidget::collapseAll()
    {
        m_treeView->collapseAll();
        expandRoot();
    }

    void GmodTreeWidget::setupTreeView()
    {
        m_treeView->setModel(m_model);
        m_treeView->setHeaderHidden(true);
        m_treeView->installEventFilter(this);
        m_treeView->setItemDelegate(new GmodTreeItemDelegate{ m_treeView });
    }

    void GmodTreeWidget::setupLayout()
    {
        auto* layout{ new QVBoxLayout{ this } };
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_treeView);
    }

    void GmodTreeWidget::setupConnections()
    {
        QObject::connect(m_treeView, &QTreeView::collapsed, this, &GmodTreeWidget::onRootCollapsed);
        QObject::connect(
            m_treeView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &GmodTreeWidget::onCurrentRowChanged);
    }

    void GmodTreeWidget::expandRoot()
    {
        m_treeView->setExpanded(m_model->index(0, 0), true);
    }

    bool GmodTreeWidget::eventFilter(QObject* watched, QEvent* event)
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

        return QWidget::eventFilter(watched, event);
    }

    void GmodTreeWidget::onRootCollapsed(const QModelIndex& index)
    {
        if (!index.parent().isValid())
        {
            m_treeView->setExpanded(index, true);
        }
    }

    void GmodTreeWidget::onCurrentRowChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
    {
        const auto* node{ current.data(GmodTreeModel::NodeRole).value<const dnv::vista::sdk::GmodNode*>() };
        emit nodeSelectionChanged(node);
    }
} // namespace qvistools::gui
