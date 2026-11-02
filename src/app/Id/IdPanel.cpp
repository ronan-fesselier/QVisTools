#include "IdPanel.h"

#include "LocalIdBuilderWidget.h"

#include <QVBoxLayout>

namespace qvistools
{
    IdPanel::IdPanel(dnv::vista::sdk::VisVersion version, QWidget* parent)
        : QGroupBox{ tr("Local ID/Universal ID"), parent },
          m_builder{ new LocalIdBuilderWidget{ version, this } }
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
        m_builder->setVisVersion(version);
    }

    void IdPanel::setupLayout()
    {
        auto* layout{ new QVBoxLayout{ this } };
        layout->addWidget(m_builder);
        layout->addStretch(1);
    }

    void IdPanel::setupConnections()
    {
        QObject::connect(
            m_builder, &LocalIdBuilderWidget::primaryItemUnmapRequested, this, &IdPanel::primaryItemUnmapRequested);
        QObject::connect(
            m_builder, &LocalIdBuilderWidget::secondaryItemUnmapRequested, this, &IdPanel::secondaryItemUnmapRequested);
    }
} // namespace qvistools
