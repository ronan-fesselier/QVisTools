#pragma once

#include <dnv/vista/sdk/core/VisVersions.h>

#include <QGroupBox>

#include <array>
#include <optional>

class QLabel;

namespace dnv::vista::sdk
{
    class GmodPath;
} // namespace dnv::vista::sdk

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

        dnv::vista::sdk::VisVersion m_version;

        QLabel* m_versionLabel;
        GmodPathItemSlot* m_primaryItemSlot;
        GmodPathItemSlot* m_secondaryItemSlot;

        std::array<MetadataTagRow*, 8> m_metadataRows;
        int m_metadataColumn{ 0 };

    signals:
        void primaryItemUnmapRequested();
        void secondaryItemUnmapRequested();
    };
} // namespace qvistools
