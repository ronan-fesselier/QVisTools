#pragma once

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;

namespace qvistools::gui
{
    class UniversalIdWidget final : public QWidget
    {
        Q_OBJECT

    public:
        explicit UniversalIdWidget(QWidget* parent = nullptr);

    public slots:
        void setLocalIdText(const QString& localIdText);

    private:
        void setupLayout();
        void setupConnections();

        void rebuild();

        QString m_localIdText;

        QLineEdit* m_imoNumberEdit;
        QLineEdit* m_universalIdValue;
        QPushButton* m_copyButton;
        QLabel* m_errorsValue;
        QString m_universalIdText;
    };
} // namespace qvistools::gui
