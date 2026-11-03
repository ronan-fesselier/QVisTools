#pragma once

#include <viscore/id/ParseId.h>

#include <QGroupBox>

class QLineEdit;
class QPushButton;

namespace qvistools
{
    class ParseIdWidget final : public QGroupBox
    {
        Q_OBJECT

    public:
        explicit ParseIdWidget(QWidget* parent = nullptr);

    private:
        void setupLayout();
        void setupConnections();
        void parseExistingId();

        QLineEdit* m_existingIdEdit;
        QPushButton* m_parseButton;

    signals:
        void idParsed(const viscore::id::ParsedId& parsed);
        void errorsChanged(const QStringList& errors);
    };
} // namespace qvistools
