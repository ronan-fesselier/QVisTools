#pragma once

#include <dnv/vista/sdk/core/GmodPath.h>

#include <QWidget>

#include <array>
#include <optional>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;

namespace dnv::vista::sdk
{
    class LocalId;
    enum class CodebookName : unsigned char;
} // namespace dnv::vista::sdk

namespace qvistools::gui
{
    class LocalIdBuilderWidget final : public QWidget
    {
        Q_OBJECT

    public:
        static constexpr int TagCount{ 8 };

        explicit LocalIdBuilderWidget(QWidget* parent = nullptr);

        void setPath(const std::optional<dnv::vista::sdk::GmodPath>& path);
        void setSecondaryItemPath(const std::optional<dnv::vista::sdk::GmodPath>& path);
        void setLocalId(const dnv::vista::sdk::LocalId& localId);
        void setParsingError(const QString& message);

    private:
        void setupLayout();
        void setupConnections();

        void rebuildLocalId();

        std::optional<dnv::vista::sdk::GmodPath> m_primaryItem;
        std::optional<dnv::vista::sdk::GmodPath> m_secondaryItem;

        QLabel* m_primaryItemValue;
        std::array<QComboBox*, TagCount> m_tagCombos;
        QLabel* m_secondaryItemValue;
        QPushButton* m_setSecondaryItemButton;
        QPushButton* m_clearSecondaryItemButton;
        QCheckBox* m_verboseCheckBox;
        QLineEdit* m_localIdValue;
        QPushButton* m_copyButton;
        QLabel* m_errorsValue;
        QString m_localIdText;
        bool m_suppressRebuild{ false };

    signals:
        void secondaryItemRequested();
    };
} // namespace qvistools::gui
