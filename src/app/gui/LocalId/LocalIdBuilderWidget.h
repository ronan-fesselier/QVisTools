#pragma once

#include <dnv/vista/sdk/core/CodebookName.h>
#include <dnv/vista/sdk/core/GmodPath.h>
#include <dnv/vista/sdk/core/VisVersions.h>

#include <QGroupBox>

#include <array>
#include <optional>

class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

namespace qvistools::gui
{
    class GmodPathItemSlot;
    class MetadataTagRow;

    /**
     * @brief The Local ID builder panel: the path bar ("/dnv-v2/vis-<version>/+Primary/
     *        sec/+Secondary/meta") followed by one row per codebook
     */
    class LocalIdBuilderWidget final : public QGroupBox
    {
        Q_OBJECT

    public:
        explicit LocalIdBuilderWidget(dnv::vista::sdk::VisVersion version, QWidget* parent = nullptr);

        /**
         * @brief Fill (or clear, if path is nullopt) the primary item slot
         */
        void setPrimaryItem(const std::optional<dnv::vista::sdk::GmodPath>& path);

        /**
         * @brief Fill (or clear, if path is nullopt) the secondary item slot
         */
        void setSecondaryItem(const std::optional<dnv::vista::sdk::GmodPath>& path);

        /**
         * @brief Update the displayed VIS version (e.g. after GmodExplorer's version
         *        combo changes) without resetting the item selections
         */
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

        QLineEdit* m_imoEdit;
        QLineEdit* m_universalIdValue;
        QPushButton* m_universalIdCopyButton;
        QString m_universalIdText;

        QLabel* m_localIdErrorsValue;
        QLabel* m_universalIdErrorsValue;

    signals:
        void primaryItemUnmapRequested();
        void secondaryItemUnmapRequested();
        void localIdChanged(const QString& localIdText);
    };
} // namespace qvistools::gui
