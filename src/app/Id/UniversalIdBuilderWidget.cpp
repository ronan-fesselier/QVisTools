#include "UniversalIdBuilderWidget.h"

#include "app/detail/QStringUtils.h"
#include "IdLayoutConstants.h"

#include <viscore/id/UniversalId.h>

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include <utility>

namespace qvistools
{
    UniversalIdBuilderWidget::UniversalIdBuilderWidget(dnv::vista::sdk::VisVersion version, QWidget* parent)
        : QGroupBox{ tr("Universal ID builder"), parent },
          m_version{ version },
          m_imoEdit{ new QLineEdit{ this } },
          m_verboseCheckBox{ new QCheckBox{ tr("Verbose"), this } },
          m_universalIdValue{ new QLineEdit{ this } },
          m_copyButton{ new QPushButton{ tr("Copy"), this } }
    {
        setupLayout();
        setupConnections();
    }

    void UniversalIdBuilderWidget::setVisVersion(dnv::vista::sdk::VisVersion version)
    {
        m_version = version;
        rebuildUniversalId();
    }

    void UniversalIdBuilderWidget::setImoNumberText(const QString& imoNumberText)
    {
        m_imoEdit->setText(imoNumberText);
    }

    void UniversalIdBuilderWidget::setVerbose(bool verbose)
    {
        m_verboseCheckBox->setChecked(verbose);
    }

    void UniversalIdBuilderWidget::setLocalIdBuilder(
        const std::optional<dnv::vista::sdk::LocalIdBuilder>& localIdBuilder)
    {
        m_localIdBuilder = localIdBuilder;
        rebuildUniversalId();
    }

    void UniversalIdBuilderWidget::setupLayout()
    {
        m_imoEdit->setPlaceholderText(tr("IMO number"));
        m_imoEdit->setMaximumWidth(120);

        m_universalIdValue->setReadOnly(true);

        auto* imoLabel{ new QLabel{ tr("IMO"), this } };
        imoLabel->setFixedWidth(ID_LABEL_WIDTH);

        auto* imoLayout{ new QHBoxLayout };
        imoLayout->addWidget(imoLabel);
        imoLayout->addWidget(m_imoEdit);
        imoLayout->addStretch(1);

        auto* universalIdLabel{ new QLabel{ tr("Universal ID"), this } };
        universalIdLabel->setFixedWidth(ID_LABEL_WIDTH);

        auto* universalIdLayout{ new QHBoxLayout };
        universalIdLayout->addWidget(universalIdLabel);
        universalIdLayout->addWidget(m_universalIdValue, 1);
        universalIdLayout->addWidget(m_verboseCheckBox);
        universalIdLayout->addWidget(m_copyButton);

        auto* layout{ new QVBoxLayout{ this } };
        layout->addLayout(imoLayout);
        layout->addLayout(universalIdLayout);
    }

    void UniversalIdBuilderWidget::setupConnections()
    {
        QObject::connect(m_imoEdit, &QLineEdit::textChanged, this, &UniversalIdBuilderWidget::rebuildUniversalId);
        QObject::connect(m_verboseCheckBox, &QCheckBox::toggled, this, &UniversalIdBuilderWidget::rebuildUniversalId);

        QObject::connect(m_copyButton, &QPushButton::clicked, this, [this]() {
            QApplication::clipboard()->setText(m_universalIdText);
        });
    }

    void UniversalIdBuilderWidget::rebuildUniversalId()
    {
        m_universalIdText.clear();
        m_universalIdValue->clear();
        m_copyButton->setEnabled(false);

        const auto imoText{ m_imoEdit->text().trimmed().toStdString() };
        if (imoText.empty() || !m_localIdBuilder.has_value())
        {
            emit errorsChanged({});
            return;
        }

        const auto builder{ std::move(dnv::vista::sdk::LocalIdBuilder{ *m_localIdBuilder })
                                .withVerboseMode(m_verboseCheckBox->isChecked()) };
        const auto result{ viscore::id::buildUniversalId(m_version, imoText, builder) };

        if (!result.universalId.has_value())
        {
            m_universalIdValue->setText(tr("(invalid)"));

            QStringList lines;
            for (const auto& error : result.errors)
            {
                lines << detail::toQString(error.message);
            }
            emit errorsChanged(lines);
            return;
        }

        m_universalIdText = detail::toQString(*result.universalId);
        m_universalIdValue->setText(m_universalIdText);
        m_copyButton->setEnabled(true);

        emit errorsChanged({});
    }
} // namespace qvistools
