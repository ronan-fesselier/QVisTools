#pragma once

#include <QWidget>

class QFormLayout;
class QLabel;

namespace dnv::vista::sdk
{
    class GmodNode;
} // namespace dnv::vista::sdk

namespace qvistools::gui
{
    class GmodNodeDetailWidget final : public QWidget
    {
        Q_OBJECT

    public:
        explicit GmodNodeDetailWidget(QWidget* parent = nullptr);

        void setNode(const dnv::vista::sdk::GmodNode* node);

    private:
        void setupLayout();

        QLabel* m_codeValue;
        QLabel* m_nameValue;
        QLabel* m_categoryValue;
        QLabel* m_definitionValue;
    };
} // namespace qvistools::gui
