#pragma once

#include <QWidget>

#include <optional>

class QFormLayout;
class QLabel;
class QPushButton;

namespace dnv::vista::sdk
{
    class GmodNode;
    class GmodPath;
} // namespace dnv::vista::sdk

namespace qvistools::gui
{
    class GmodNodeDetailWidget final : public QWidget
    {
        Q_OBJECT

    public:
        explicit GmodNodeDetailWidget(QWidget* parent = nullptr);

        void setNode(const dnv::vista::sdk::GmodNode* node, const std::optional<dnv::vista::sdk::GmodPath>& path);

    private:
        void setupLayout();
        void clear();

        QLabel* m_codeValue;
        QLabel* m_nameValue;
        QLabel* m_categoryValue;
        QLabel* m_definitionValue;

        QLabel* m_shortPathValue;
        QLabel* m_fullPathValue;
        QPushButton* m_copyShortPathButton;
        QPushButton* m_copyFullPathButton;

        QString m_shortPathText;
        QString m_fullPathText;
    };
} // namespace qvistools::gui
