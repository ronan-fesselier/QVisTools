#include "GmodNodeDetailWidget.h"

#include "detail/StringUtils.h"
#include "gui/Shared/GmodChipRenderer.h"

#include <dnv/vista/sdk/core/GmodNode.h>
#include <dnv/vista/sdk/core/GmodPath.h>

#include <QApplication>
#include <QClipboard>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

namespace qvistools::gui
{
    GmodNodeDetailWidget::GmodNodeDetailWidget(QWidget* parent)
        : QWidget{ parent },
          m_codeValue{ new QLabel{ this } },
          m_nameValue{ new QLabel{ this } },
          m_categoryValue{ new QLabel{ this } },
          m_definitionValue{ new QLabel{ this } },
          m_shortPathValue{ new QLabel{ this } },
          m_fullPathValue{ new QLabel{ this } },
          m_copyShortPathButton{ new QPushButton{ tr("Copy"), this } },
          m_copyFullPathButton{ new QPushButton{ tr("Copy"), this } }
    {
        setupLayout();
        setNode(nullptr, std::nullopt);
    }

    void GmodNodeDetailWidget::setNode(
        const dnv::vista::sdk::GmodNode* node, const std::optional<dnv::vista::sdk::GmodPath>& path)
    {
        if (node == nullptr)
        {
            clear();
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

        if (path.has_value())
        {
            m_shortPathText = detail::toQString(path->toString());
            m_fullPathText = detail::toQString(path->toFullPathString());
            m_shortPathValue->setText(m_shortPathText);
            m_fullPathValue->setText(m_fullPathText);
            m_copyShortPathButton->setEnabled(true);
            m_copyFullPathButton->setEnabled(true);
        }
        else
        {
            m_shortPathValue->setText(tr("(unavailable)"));
            m_fullPathValue->setText(tr("(unavailable)"));
            m_copyShortPathButton->setEnabled(false);
            m_copyFullPathButton->setEnabled(false);
            m_shortPathText.clear();
            m_fullPathText.clear();
        }
    }

    void GmodNodeDetailWidget::setupLayout()
    {
        m_nameValue->setWordWrap(true);
        m_definitionValue->setWordWrap(true);
        m_shortPathValue->setWordWrap(true);
        m_fullPathValue->setWordWrap(true);

        for (auto* label :
             { m_codeValue, m_nameValue, m_categoryValue, m_definitionValue, m_shortPathValue, m_fullPathValue })
        {
            label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        }

        auto* definitionScroll{ new QScrollArea{ this } };
        definitionScroll->setWidget(m_definitionValue);
        definitionScroll->setWidgetResizable(true);
        definitionScroll->setMaximumHeight(120);
        definitionScroll->setFrameShape(QFrame::NoFrame);
        definitionScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        auto* formLayout{ new QFormLayout };
        formLayout->setContentsMargins(0, 0, 0, 0);
        formLayout->setSpacing(10);

        formLayout->addRow(tr("Code:"), m_codeValue);
        formLayout->addRow(tr("Name:"), m_nameValue);
        formLayout->addRow(tr("Category:"), m_categoryValue);
        formLayout->addRow(tr("Definition:"), definitionScroll);

        auto* shortPathLayout{ new QHBoxLayout };
        shortPathLayout->addWidget(m_shortPathValue, 1);
        shortPathLayout->addWidget(m_copyShortPathButton);
        formLayout->addRow(tr("Short path:"), shortPathLayout);

        auto* fullPathLayout{ new QHBoxLayout };
        fullPathLayout->addWidget(m_fullPathValue, 1);
        fullPathLayout->addWidget(m_copyFullPathButton);
        formLayout->addRow(tr("Full path:"), fullPathLayout);

        auto* layout{ new QVBoxLayout{ this } };
        layout->setContentsMargins(12, 12, 12, 12);
        layout->addLayout(formLayout);
        layout->addStretch(1);

        QObject::connect(m_copyShortPathButton, &QPushButton::clicked, this, [this]() {
            QApplication::clipboard()->setText(m_shortPathText);
        });
        QObject::connect(m_copyFullPathButton, &QPushButton::clicked, this, [this]() {
            QApplication::clipboard()->setText(m_fullPathText);
        });
    }

    void GmodNodeDetailWidget::clear()
    {
        m_codeValue->clear();
        m_nameValue->clear();
        m_categoryValue->clear();
        m_definitionValue->clear();
        m_shortPathValue->clear();
        m_fullPathValue->clear();
        m_copyShortPathButton->setEnabled(false);
        m_copyFullPathButton->setEnabled(false);
        m_shortPathText.clear();
        m_fullPathText.clear();
    }
} // namespace qvistools::gui
