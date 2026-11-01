#include "MetadataTagRow.h"

#include "app/gui/detail/QStringUtils.h"

#include <QCompleter>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

namespace qvistools::gui
{
    namespace
    {
        QLabel* makePrefixLabel(dnv::vista::sdk::CodebookName name, QWidget* parent)
        {
            const auto prefix{ dnv::vista::sdk::CodebookNames::toPrefix(name) };
            auto* label{ new QLabel{ QStringLiteral("/ %1 -").arg(detail::toQString(prefix)), parent } };
            label->setStyleSheet(QStringLiteral("color: palette(window-text);"));
            label->setFixedWidth(70);
            return label;
        }
    } // namespace

    MetadataTagRow::MetadataTagRow(
        dnv::vista::sdk::CodebookName name, const std::vector<std::string>& standardValues, QWidget* parent)
        : QWidget{ parent },
          m_name{ name },
          m_valueCombo{ nullptr },
          m_valueEdit{ nullptr }
    {
        auto* layout{ new QHBoxLayout{ this } };
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(4);
        layout->addWidget(makePrefixLabel(name, this));

        if (standardValues.empty())
        {
            m_valueEdit = new QLineEdit{ this };
            m_valueEdit->setPlaceholderText(tr("Type here to select"));
            layout->addWidget(m_valueEdit, 1);

            QObject::connect(m_valueEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
                emit valueChanged(m_name, text);
            });

            return;
        }

        m_valueCombo = new QComboBox{ this };
        m_valueCombo->setEditable(true);
        m_valueCombo->setInsertPolicy(QComboBox::NoInsert);
        m_valueCombo->lineEdit()->setPlaceholderText(tr("Type here to select"));

        for (const auto& value : standardValues)
        {
            m_valueCombo->addItem(detail::toQString(value));
        }
        m_valueCombo->setCurrentIndex(-1);

        auto* completer{ new QCompleter{ m_valueCombo->model(), m_valueCombo } };
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        m_valueCombo->setCompleter(completer);

        layout->addWidget(m_valueCombo, 1);

        QObject::connect(m_valueCombo, &QComboBox::currentTextChanged, this, [this](const QString& text) {
            emit valueChanged(m_name, text);
        });
    }

    QString MetadataTagRow::value() const
    {
        return m_valueCombo != nullptr ? m_valueCombo->currentText() : m_valueEdit->text();
    }
} // namespace qvistools::gui
