#pragma once

#include <dnv/vista/sdk/core/CodebookName.h>

#include <QWidget>

#include <string>
#include <vector>

class QComboBox;
class QLineEdit;

namespace qvistools::gui
{
    /**
     * @brief One metadata tag row in the Local ID builder: "/<prefix> - [value]",
     *        e.g. "/qty - gauge.pressure"
     */
    class MetadataTagRow final : public QWidget
    {
        Q_OBJECT

    public:
        MetadataTagRow(
            dnv::vista::sdk::CodebookName name,
            const std::vector<std::string>& standardValues,
            QWidget* parent = nullptr);

        [[nodiscard]] dnv::vista::sdk::CodebookName codebookName() const noexcept { return m_name; }
        [[nodiscard]] QString value() const;

    private:
        dnv::vista::sdk::CodebookName m_name;
        QComboBox* m_valueCombo;
        QLineEdit* m_valueEdit;

    signals:
        void valueChanged(dnv::vista::sdk::CodebookName name, const QString& value);
    };
} // namespace qvistools::gui
