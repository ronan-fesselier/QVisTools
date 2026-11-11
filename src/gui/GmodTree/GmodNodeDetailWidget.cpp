#include "GmodNodeDetailWidget.h"

#include "detail/StringUtils.h"
#include "GmodChipRenderer.h"

#include <dnv/vista/sdk/core/GmodNode.h>

#include <QFormLayout>
#include <QLabel>

namespace qvistools::gui
{
    GmodNodeDetailWidget::GmodNodeDetailWidget(QWidget* parent)
        : QWidget{ parent },
          m_codeValue{ new QLabel{ this } },
          m_nameValue{ new QLabel{ this } },
          m_categoryValue{ new QLabel{ this } },
          m_definitionValue{ new QLabel{ this } }
    {
        setupLayout();
        setNode(nullptr);
    }

    void GmodNodeDetailWidget::setNode(const dnv::vista::sdk::GmodNode* node)
    {
        if (node == nullptr)
        {
            m_codeValue->setText(QString{});
            m_nameValue->setText(QString{});
            m_categoryValue->setText(QString{});
            m_definitionValue->setText(QString{});
            return;
        }

        const auto& metadata{ node->metadata() };

        m_codeValue->setText(detail::toQString(node->code()));

        const auto name{ metadata.commonName().has_value() ? *metadata.commonName() : std::string{ metadata.name() } };
        m_nameValue->setText(detail::toQString(name));

        m_categoryValue->setText(chip::categoryName(chip::categoryOf(*node)));

        if (metadata.commonDefinition().has_value())
        {
            m_definitionValue->setText(detail::toQString(*metadata.commonDefinition()));
        }
        else if (metadata.definition().has_value())
        {
            m_definitionValue->setText(detail::toQString(*metadata.definition()));
        }
        else
        {
            m_definitionValue->setText(QString{});
        }
    }

    void GmodNodeDetailWidget::setupLayout()
    {
        m_nameValue->setWordWrap(true);
        m_definitionValue->setWordWrap(true);

        auto* layout{ new QFormLayout{ this } };
        layout->addRow(tr("Code:"), m_codeValue);
        layout->addRow(tr("Name:"), m_nameValue);
        layout->addRow(tr("Category:"), m_categoryValue);
        layout->addRow(tr("Definition:"), m_definitionValue);
    }
} // namespace qvistools::gui
