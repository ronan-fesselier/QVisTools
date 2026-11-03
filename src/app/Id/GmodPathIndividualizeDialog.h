#pragma once

#include <dnv/vista/sdk/core/GmodPath.h>

#include <QDialog>

#include <optional>
#include <vector>

class QComboBox;
class QLabel;
class QSpinBox;
class QVBoxLayout;

namespace qvistools
{
    class GmodPathIndividualizeDialog final : public QDialog
    {
        Q_OBJECT

    public:
        explicit GmodPathIndividualizeDialog(
            dnv::vista::sdk::GmodPath path,
            std::optional<std::size_t> onlySetIndex = std::nullopt,
            QWidget* parent = nullptr);

        const dnv::vista::sdk::GmodPath& result() const noexcept;

    private:
        struct SetCard
        {
            std::size_t setIndex;
            QSpinBox* numberEdit;
            QComboBox* sideCombo;
            QComboBox* verticalCombo;
            QComboBox* transverseCombo;
            QComboBox* longitudinalCombo;
        };

        void setupSetCard(QVBoxLayout* cardsLayout, std::size_t setIndex);
        void updatePreview();
        void accept() override;

        dnv::vista::sdk::GmodPath m_sourcePath;
        dnv::vista::sdk::GmodPath m_resultPath;

        std::vector<SetCard> m_cards;
        QLabel* m_previewValue;
        QLabel* m_errorValue;
    };
} // namespace qvistools
