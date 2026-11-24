#include "IdPanel.h"

#include "app/detail/QStringUtils.h"
#include "IdErrorsWidget.h"
#include "LocalIdBuilderWidget.h"
#include "ParseIdWidget.h"
#include "UniversalIdBuilderWidget.h"

#include <QVBoxLayout>

namespace qvistools
{
    IdPanel::IdPanel(dnv::vista::sdk::VisVersion version, QWidget* parent)
        : QGroupBox{ tr("Local ID/Universal ID"), parent },
          m_version{ version },
          m_parseId{ new ParseIdWidget{ this } },
          m_builder{ new LocalIdBuilderWidget{ version, this } },
          m_universalIdBuilder{ new UniversalIdBuilderWidget{ version, this } },
          m_errors{ new IdErrorsWidget{ this } }
    {
        setupLayout();
        setupConnections();
    }

    void IdPanel::setPrimaryItem(const std::optional<dnv::vista::sdk::GmodPath>& path)
    {
        m_builder->setPrimaryItem(path);
    }

    void IdPanel::setSecondaryItem(const std::optional<dnv::vista::sdk::GmodPath>& path)
    {
        m_builder->setSecondaryItem(path);
    }

    void IdPanel::setVisVersion(dnv::vista::sdk::VisVersion version)
    {
        m_version = version;
        m_builder->setVisVersion(version);
        m_universalIdBuilder->setVisVersion(version);
    }

    void IdPanel::setupLayout()
    {
        auto* layout{ new QVBoxLayout{ this } };
        layout->addWidget(m_parseId);
        layout->addWidget(m_builder);
        layout->addWidget(m_universalIdBuilder);
        layout->addWidget(m_errors);
        layout->addStretch(1);
    }

    void IdPanel::setupConnections()
    {
        QObject::connect(m_parseId, &ParseIdWidget::errorsChanged, m_errors, &IdErrorsWidget::setParseErrors);
        QObject::connect(m_builder, &LocalIdBuilderWidget::errorsChanged, m_errors, &IdErrorsWidget::setLocalIdErrors);
        QObject::connect(
            m_universalIdBuilder,
            &UniversalIdBuilderWidget::errorsChanged,
            m_errors,
            &IdErrorsWidget::setUniversalIdErrors);

        QObject::connect(
            m_builder,
            &LocalIdBuilderWidget::localIdBuilderChanged,
            m_universalIdBuilder,
            &UniversalIdBuilderWidget::setLocalIdBuilder);

        QObject::connect(
            m_builder, &LocalIdBuilderWidget::primaryItemUnmapRequested, this, &IdPanel::primaryItemUnmapRequested);
        QObject::connect(
            m_builder, &LocalIdBuilderWidget::secondaryItemUnmapRequested, this, &IdPanel::secondaryItemUnmapRequested);
        QObject::connect(m_builder, &LocalIdBuilderWidget::revealPathRequested, this, &IdPanel::revealPathRequested);

        QObject::connect(m_parseId, &ParseIdWidget::idParsed, this, [this](const viscore::id::ParsedId& parsed) {
            if (parsed.version != m_version)
            {
                emit existingIdVersionParsed(parsed.version);
            }

            m_builder->applyParsedId(parsed.primaryItem, parsed.secondaryItem, parsed.tags, parsed.verbose);
            m_universalIdBuilder->setImoNumberText(
                parsed.imoNumberText.has_value() ? detail::toQString(*parsed.imoNumberText) : QString{});
            m_universalIdBuilder->setVerbose(parsed.verbose);
        });
    }
} // namespace qvistools
