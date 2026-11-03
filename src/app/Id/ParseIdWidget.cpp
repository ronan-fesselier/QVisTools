#include "ParseIdWidget.h"

#include "app/detail/QStringUtils.h"
#include "IdLayoutConstants.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>

namespace qvistools
{
    ParseIdWidget::ParseIdWidget(QWidget* parent)
        : QGroupBox{ tr("Existing Local/Universal ID"), parent },
          m_existingIdEdit{ new QLineEdit{ this } },
          m_parseButton{ new QPushButton{ tr("Parse"), this } }
    {
        setupLayout();
        setupConnections();
    }

    void ParseIdWidget::setupLayout()
    {
        m_existingIdEdit->setPlaceholderText(tr("Existing Local or Universal ID"));

        auto* label{ new QLabel{ tr("Existing ID"), this } };
        label->setFixedWidth(ID_LABEL_WIDTH);

        auto* layout{ new QHBoxLayout{ this } };
        layout->addWidget(label);
        layout->addWidget(m_existingIdEdit, 1);
        layout->addWidget(m_parseButton);
    }

    void ParseIdWidget::setupConnections()
    {
        QObject::connect(m_parseButton, &QPushButton::clicked, this, &ParseIdWidget::parseExistingId);
        QObject::connect(m_existingIdEdit, &QLineEdit::returnPressed, this, &ParseIdWidget::parseExistingId);
    }

    void ParseIdWidget::parseExistingId()
    {
        const auto text{ m_existingIdEdit->text().trimmed().toStdString() };
        if (text.empty())
        {
            emit errorsChanged({});
            return;
        }

        const auto result{ viscore::id::parseExistingId(text) };
        if (!result.id.has_value())
        {
            QStringList lines;
            for (const auto& error : result.errors)
            {
                lines << detail::toQString(error);
            }
            emit errorsChanged(lines);
            return;
        }

        emit errorsChanged({});
        emit idParsed(*result.id);
    }
} // namespace qvistools
