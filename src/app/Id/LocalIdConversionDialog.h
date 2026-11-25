#pragma once

#include <dnv/vista/sdk/core/LocalIdBuilder.h>
#include <dnv/vista/sdk/core/VisVersions.h>

#include <QDialog>

#include <optional>

class QComboBox;
class QLabel;

namespace qvistools
{
    class LocalIdConversionDialog final : public QDialog
    {
        Q_OBJECT

    public:
        explicit LocalIdConversionDialog(dnv::vista::sdk::LocalIdBuilder sourceBuilder, QWidget* parent = nullptr);

        const dnv::vista::sdk::LocalIdBuilder& result() const noexcept;

    private:
        void updatePreview();
        void accept() override;

        dnv::vista::sdk::LocalIdBuilder m_sourceBuilder;
        std::optional<dnv::vista::sdk::LocalIdBuilder> m_resultBuilder;

        QComboBox* m_targetVersionCombo;
        QLabel* m_beforeValue;
        QLabel* m_afterValue;
        QLabel* m_errorValue;
    };
} // namespace qvistools
