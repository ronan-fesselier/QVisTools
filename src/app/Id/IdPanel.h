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
    class IdErrorsWidget;
    class LocalIdBuilderWidget;
    class ParseIdWidget;
    class UniversalIdBuilderWidget;

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
        void setupConnections();

        dnv::vista::sdk::VisVersion m_version;

        ParseIdWidget* m_parseId;
        LocalIdBuilderWidget* m_builder;
        UniversalIdBuilderWidget* m_universalIdBuilder;
        IdErrorsWidget* m_errors;

    signals:
        void primaryItemUnmapRequested();
        void secondaryItemUnmapRequested();
        void existingIdVersionParsed(dnv::vista::sdk::VisVersion version);
    };
} // namespace qvistools
