#pragma once

#include <dnv/vista/sdk/core/CodebookName.h>
#include <dnv/vista/sdk/core/GmodPath.h>
#include <dnv/vista/sdk/core/VisVersions.h>

#include <QGroupBox>
#include <QStringList>

#include <array>
#include <optional>

class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

namespace qvistools
{
    class GmodPathItemSlot;
    class MetadataTagRow;

    class LocalIdBuilderWidget final : public QGroupBox
    {
        Q_OBJECT

    public:
        explicit LocalIdBuilderWidget(dnv::vista::sdk::VisVersion version, QWidget* parent = nullptr);

        void setPrimaryItem(const std::optional<dnv::vista::sdk::GmodPath>& path);
        void setSecondaryItem(const std::optional<dnv::vista::sdk::GmodPath>& path);
        void setVisVersion(dnv::vista::sdk::VisVersion version);

    private:
        void setupLayout();
        void setupConnections();
        void rebuildMetadataRows();
        void rebuildLocalId();

        dnv::vista::sdk::VisVersion m_version;
        std::optional<dnv::vista::sdk::GmodPath> m_primaryPath;
        std::optional<dnv::vista::sdk::GmodPath> m_secondaryPath;

        QLabel* m_versionLabel;
        GmodPathItemSlot* m_primaryItemSlot;
        GmodPathItemSlot* m_secondaryItemSlot;

        std::array<MetadataTagRow*, 8> m_metadataRows;
        int m_metadataColumn{ 0 };

        QCheckBox* m_verboseCheckBox;
        QLineEdit* m_localIdValue;
        QPushButton* m_copyButton;
        QString m_localIdText;

    signals:
        void primaryItemUnmapRequested();
        void secondaryItemUnmapRequested();
        void errorsChanged(const QStringList& errors);
    };
} // namespace qvistools
