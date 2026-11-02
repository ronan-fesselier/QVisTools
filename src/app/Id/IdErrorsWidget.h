#pragma once

#include <QGroupBox>
#include <QStringList>

class QLabel;

namespace qvistools
{
    class IdErrorsWidget final : public QGroupBox
    {
        Q_OBJECT

    public:
        explicit IdErrorsWidget(QWidget* parent = nullptr);

        void setLocalIdErrors(const QStringList& errors);

    private:
        QLabel* m_errorsLabel;
    };
} // namespace qvistools
