#include "GmodSearchBar.h"

#include "app/detail/QStringUtils.h"
#include "app/SdkQtTypes.h"
#include "GmodSearchResultDelegate.h"

#include <dnv/vista/sdk/core/GmodNode.h>

#include <QKeyEvent>
#include <QLineEdit>
#include <QListWidget>
#include <QTimer>
#include <QVBoxLayout>

namespace qvistools
{
    namespace
    {
        constexpr int SEARCH_DEBOUNCE_MS{ 100 };
        constexpr int MAX_DISPLAY_RESULT{ 250 };
    } // namespace

    GmodSearchBar::GmodSearchBar(const dnv::vista::sdk::Gmod& gmod, QWidget* parent)
        : QWidget{ parent },
          m_gmod{ &gmod },
          m_lineEdit{ new QLineEdit{ this } },
          m_resultsList{ new QListWidget{ this } },
          m_debounceTimer{ new QTimer{ this } }
    {
        m_lineEdit->setPlaceholderText(tr("Search Gmod nodes..."));
        m_resultsList->setHidden(true);
        m_resultsList->setItemDelegate(new GmodSearchResultDelegate{ m_resultsList });

        m_debounceTimer->setSingleShot(true);
        m_debounceTimer->setInterval(SEARCH_DEBOUNCE_MS);

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

        onTextChanged(m_lineEdit->text());
    }

    const viscore::gmod::SearchIndex& GmodSearchBar::index()
    {
        if (!m_index)
        {
            m_index = std::make_unique<viscore::gmod::SearchIndex>(*m_gmod);
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
        QObject::connect(m_lineEdit, &QLineEdit::textChanged, m_debounceTimer, QOverload<>::of(&QTimer::start));
        QObject::connect(m_debounceTimer, &QTimer::timeout, this, [this]() { onTextChanged(m_lineEdit->text()); });
        QObject::connect(m_resultsList, &QListWidget::itemActivated, this, &GmodSearchBar::onItemActivated);
    }

    void GmodSearchBar::closeResults()
    {
        m_resultsList->clear();
        m_currentMatches.clear();
        m_resultsList->setHidden(true);
        m_lineEdit->clear();
    }

    void GmodSearchBar::activateCurrentItem()
    {
        const auto row{ m_resultsList->currentRow() };
        if (row < 0 || static_cast<std::size_t>(row) >= m_currentMatches.size())
        {
            return;
        }

        const auto match{ m_currentMatches[static_cast<std::size_t>(row)] };

        closeResults();

        emit matchActivated(match);
    }

    void GmodSearchBar::onTextChanged(const QString& text)
    {
        m_resultsList->clear();
        m_currentMatches.clear();

        if (text.isEmpty())
        {
            m_resultsList->setHidden(true);
            return;
        }

        auto matches{ index().search(text.toStdString()) };
        const auto totalMatchCount{ matches.size() };
        const bool truncated{ totalMatchCount > MAX_DISPLAY_RESULT };
        if (truncated)
        {
            matches.resize(MAX_DISPLAY_RESULT);
        }

        for (const auto& match : matches)
        {
            auto* item{ new QListWidgetItem };
            item->setData(GmodSearchResultDelegate::NodeRole, QVariant::fromValue(match.node));
            item->setData(GmodSearchResultDelegate::ParentRole, QVariant::fromValue(match.parent));

            const auto& metadata{ match.node->metadata() };
            if (metadata.commonDefinition().has_value())
            {
                item->setData(Qt::ToolTipRole, detail::toQString(*metadata.commonDefinition()));
            }
            else if (metadata.definition().has_value())
            {
                item->setData(Qt::ToolTipRole, detail::toQString(*metadata.definition()));
            }

            m_resultsList->addItem(item);
        }

        if (truncated)
        {
            auto* moreItem{ new QListWidgetItem{
                tr("+%n more result(s)...", nullptr, static_cast<int>(totalMatchCount - MAX_DISPLAY_RESULT)) } };

            moreItem->setFlags(moreItem->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEnabled);
            m_resultsList->addItem(moreItem);
        }

        m_currentMatches = std::move(matches);

        m_resultsList->setHidden(m_currentMatches.empty());

        if (!m_currentMatches.empty())
        {
            m_resultsList->setCurrentRow(0);
        }
    }

    void GmodSearchBar::onItemActivated()
    {
        activateCurrentItem();
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
} // namespace qvistools
