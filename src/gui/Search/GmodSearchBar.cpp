#include "GmodSearchBar.h"

#include "core/GmodSearchIndex.h"
#include "detail/StringUtils.h"
#include "GmodSearchResultDelegate.h"
#include "gui/SdkQtTypes.h"

#include <dnv/vista/sdk/core/GmodNode.h>

#include <QKeyEvent>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>

namespace qvistools::gui
{
    GmodSearchBar::GmodSearchBar(const dnv::vista::sdk::Gmod& gmod, QWidget* parent)
        : QWidget{ parent },
          m_gmod{ &gmod },
          m_lineEdit{ new QLineEdit{ this } },
          m_resultsList{ new QListWidget{ this } }
    {
        m_lineEdit->setPlaceholderText(tr("Search Gmod nodes..."));
        m_lineEdit->setToolTip(
            tr("Search by code, name, common name, or definition.\n"
               "Down / Up navigate - Enter jumps to the node - Esc closes"));
        m_resultsList->setHidden(true);
        m_resultsList->setItemDelegate(new GmodSearchResultDelegate{ m_resultsList });

        m_lineEdit->installEventFilter(this);
        m_resultsList->installEventFilter(this);

        setupLayout();
        setupConnections();
    }

    GmodSearchBar::~GmodSearchBar() = default;

    void GmodSearchBar::setGmod(const dnv::vista::sdk::Gmod& gmod)
    {
        m_gmod = &gmod;
        m_index.reset();
        m_resultsList->clear();
        m_resultsList->setHidden(true);
    }

    bool GmodSearchBar::eventFilter(QObject* watched, QEvent* event)
    {
        if (event->type() != QEvent::KeyPress)
        {
            return QWidget::eventFilter(watched, event);
        }

        auto* keyEvent{ static_cast<QKeyEvent*>(event) };

        if (watched == m_lineEdit)
        {
            if (keyEvent->key() == Qt::Key_Escape && !m_resultsList->isHidden())
            {
                closeResults();
                return true;
            }

            if (keyEvent->key() == Qt::Key_Down && !m_resultsList->isHidden())
            {
                m_resultsList->setFocus();
                if (m_resultsList->currentItem() == nullptr && m_resultsList->count() > 0)
                {
                    m_resultsList->setCurrentRow(0);
                }
                return true;
            }

            if ((keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) && !m_resultsList->isHidden())
            {
                activateCurrentItem();
                return true;
            }
        }
        else if (watched == m_resultsList)
        {
            if (keyEvent->key() == Qt::Key_Escape)
            {
                closeResults();
                m_lineEdit->setFocus();
                return true;
            }

            if (keyEvent->key() == Qt::Key_Up && m_resultsList->currentRow() == 0)
            {
                m_lineEdit->setFocus();
                return true;
            }
        }

        return QWidget::eventFilter(watched, event);
    }

    const core::GmodSearchIndex& GmodSearchBar::index()
    {
        if (!m_index)
        {
            m_index = std::make_unique<core::GmodSearchIndex>(*m_gmod);
        }
        return *m_index;
    }

    void GmodSearchBar::setupLayout()
    {
        auto* layout{ new QVBoxLayout{ this } };
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_lineEdit);
        layout->addWidget(m_resultsList);
    }

    void GmodSearchBar::setupConnections()
    {
        QObject::connect(m_lineEdit, &QLineEdit::textChanged, this, &GmodSearchBar::onTextChanged);
        QObject::connect(m_resultsList, &QListWidget::itemActivated, this, &GmodSearchBar::onItemActivated);
    }

    void GmodSearchBar::closeResults()
    {
        m_resultsList->clear();
        m_resultsList->setHidden(true);
        m_lineEdit->clear();
    }

    void GmodSearchBar::activateCurrentItem()
    {
        auto* item{ m_resultsList->currentItem() };
        if (item == nullptr)
        {
            return;
        }

        const auto* node{ item->data(GmodSearchResultDelegate::NodeRole).value<const dnv::vista::sdk::GmodNode*>() };
        if (node == nullptr)
        {
            return;
        }

        closeResults();

        emit nodeActivated(*node);
    }

    void GmodSearchBar::onTextChanged(const QString& text)
    {
        m_resultsList->clear();

        if (text.isEmpty())
        {
            m_resultsList->setHidden(true);
            return;
        }

        const auto matches{ index().search(text.toStdString()) };

        for (const auto& match : matches)
        {
            auto* item{ new QListWidgetItem };
            item->setData(GmodSearchResultDelegate::NodeRole, QVariant::fromValue(match.node));
            item->setData(GmodSearchResultDelegate::ParentRole, QVariant::fromValue(match.parent));
            item->setData(GmodSearchResultDelegate::SkippedParentRole, QVariant::fromValue(match.skippedParent));

            const auto* namedNode{ match.skippedParent != nullptr ? match.parent : match.node };
            const auto& metadata{ namedNode->metadata() };
            if (metadata.commonDefinition().has_value())
            {
                item->setToolTip(detail::toQString(*metadata.commonDefinition()));
            }
            else if (metadata.definition().has_value())
            {
                item->setToolTip(detail::toQString(*metadata.definition()));
            }

            m_resultsList->addItem(item);
        }

        m_resultsList->setHidden(matches.empty());

        if (!matches.empty())
        {
            m_resultsList->setCurrentRow(0);
        }
    }

    void GmodSearchBar::onItemActivated()
    {
        activateCurrentItem();
    }
} // namespace qvistools::gui
