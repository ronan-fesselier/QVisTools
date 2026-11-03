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

        void setParseErrors(const QStringList& errors);
        void setLocalIdErrors(const QStringList& errors);
        void setUniversalIdErrors(const QStringList& errors);

    private:
        void refresh();

        QLabel* m_errorsLabel;
        QStringList m_parseErrors;
        QStringList m_localIdErrors;
        QStringList m_universalIdErrors;
    };
} // namespace qvistools
