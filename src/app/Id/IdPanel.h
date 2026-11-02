#pragma once

#include <dnv/vista/sdk/core/VisVersions.h>

#include <QGroupBox>

#include <optional>

namespace dnv::vista::sdk
{
    class GmodPath;
} // namespace dnv::vista::sdk

namespace qvistools
{
    class LocalIdBuilderWidget;

    class IdPanel final : public QGroupBox
    {
        Q_OBJECT

    public:
        explicit IdPanel(dnv::vista::sdk::VisVersion version, QWidget* parent = nullptr);

        void setPrimaryItem(const std::optional<dnv::vista::sdk::GmodPath>& path);
        void setSecondaryItem(const std::optional<dnv::vista::sdk::GmodPath>& path);
        void setVisVersion(dnv::vista::sdk::VisVersion version);

    private:
        void setupLayout();

        LocalIdBuilderWidget* m_builder;

    signals:
        void primaryItemUnmapRequested();
        void secondaryItemUnmapRequested();
    };
} // namespace qvistools
