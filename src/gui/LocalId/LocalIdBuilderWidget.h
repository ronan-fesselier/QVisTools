#pragma once

#include <dnv/vista/sdk/core/GmodPath.h>

#include <QWidget>

#include <array>
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

class QAction;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QToolButton;

namespace dnv::vista::sdk
{
    class LocalId;
    enum class CodebookName : unsigned char;
    enum class VisVersion : std::uint8_t;
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

        [[nodiscard]] const QString& localIdText() const noexcept { return m_localIdText; }

    private:
        void setupLayout();
        void setupConnections();

        void rebuildLocalId();
        void updateConvertMenu();

        std::optional<dnv::vista::sdk::GmodPath> m_primaryItem;
        std::optional<dnv::vista::sdk::GmodPath> m_secondaryItem;

        QLabel* m_primaryItemValue;
        QPushButton* m_setPrimaryItemButton;
        std::array<QComboBox*, TagCount> m_tagCombos;
        QLabel* m_secondaryItemValue;
        QPushButton* m_setSecondaryItemButton;
        QPushButton* m_clearSecondaryItemButton;
        QCheckBox* m_verboseCheckBox;
        QLineEdit* m_localIdValue;
        QPushButton* m_copyButton;
        QToolButton* m_convertButton;
        QMenu* m_convertMenu;
        std::vector<std::pair<dnv::vista::sdk::VisVersion, QAction*>> m_convertActions;
        QLabel* m_errorsValue;
        QString m_localIdText;
        bool m_suppressRebuild{ false };

    signals:
        void primaryItemRequested();
        void secondaryItemRequested();
        void convertRequested(dnv::vista::sdk::VisVersion targetVersion);
    };
} // namespace qvistools::gui
