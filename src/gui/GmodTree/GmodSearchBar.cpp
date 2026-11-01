#include "GmodSearchBar.h"

#include "core/GmodSearchIndex.h"
#include "gui/SdkQtTypes.h"
#include "detail/StringUtils.h"

#include <dnv/vista/sdk/core/GmodNode.h>

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
        m_resultsList->setHidden(true);

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
            const auto* node{ match.node };
            auto* item{ new QListWidgetItem{ detail::toQString(node->code()) + QStringLiteral(" - ") +
                                             detail::toQString(node->metadata().name()) } };
            item->setData(Qt::UserRole, QVariant::fromValue(node));
            m_resultsList->addItem(item);
        }

        m_resultsList->setHidden(matches.empty());
    }

    void GmodSearchBar::onItemActivated()
    {
        auto* item{ m_resultsList->currentItem() };
        if (item == nullptr)
        {
            return;
        }

        const auto* node{ item->data(Qt::UserRole).value<const dnv::vista::sdk::GmodNode*>() };
        if (node == nullptr)
        {
            return;
        }

        m_resultsList->clear();
        m_resultsList->setHidden(true);
        m_lineEdit->clear();

        emit nodeActivated(*node);
    }
} // namespace qvistools::gui
