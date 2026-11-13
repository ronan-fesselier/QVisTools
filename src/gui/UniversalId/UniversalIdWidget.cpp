#include "UniversalIdWidget.h"

#include "core/UniversalIdUtils.h"
#include "detail/StringUtils.h"

#include <QApplication>
#include <QClipboard>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>

namespace qvistools::gui
{
    UniversalIdWidget::UniversalIdWidget(QWidget* parent)
        : QWidget{ parent },
          m_imoNumberEdit{ new QLineEdit{ this } },
          m_universalIdValue{ new QLineEdit{ this } },
          m_copyButton{ new QPushButton{ tr("Copy"), this } },
          m_errorsValue{ new QLabel{ this } }
    {
        m_imoNumberEdit->setPlaceholderText(tr("e.g. IMO1234567"));
        m_universalIdValue->setReadOnly(true);
        m_copyButton->setEnabled(false);

        setupLayout();
        setupConnections();

        rebuild();
    }

    void UniversalIdWidget::setLocalIdText(const QString& localIdText)
    {
        m_localIdText = localIdText;
        rebuild();
    }

    void UniversalIdWidget::setupLayout()
    {
        auto* formLayout{ new QFormLayout };
        formLayout->addRow(tr("IMO number:"), m_imoNumberEdit);

        auto* universalIdLayout{ new QHBoxLayout };
        universalIdLayout->addWidget(m_universalIdValue, 1);
        universalIdLayout->addWidget(m_copyButton);
        formLayout->addRow(tr("UniversalId:"), universalIdLayout);

        m_errorsValue->setWordWrap(true);
        m_errorsValue->setTextInteractionFlags(Qt::TextSelectableByMouse);

        QPalette errorPalette{ m_errorsValue->palette() };
        errorPalette.setColor(QPalette::WindowText, Qt::red);
        m_errorsValue->setPalette(errorPalette);

        auto* errorsLayout{ new QHBoxLayout };
        errorsLayout->addWidget(m_errorsValue, 1);
        formLayout->addRow(tr("Errors:"), errorsLayout);

        auto* layout{ new QVBoxLayout{ this } };
        layout->addLayout(formLayout);
    }

    void UniversalIdWidget::setupConnections()
    {
        QObject::connect(m_imoNumberEdit, &QLineEdit::textChanged, this, &UniversalIdWidget::rebuild);

        QObject::connect(m_copyButton, &QPushButton::clicked, this, [this]() {
            QApplication::clipboard()->setText(m_universalIdText);
        });
    }

    void UniversalIdWidget::rebuild()
    {
        m_errorsValue->clear();
        m_universalIdText.clear();
        m_copyButton->setEnabled(false);

        const auto imoText{ m_imoNumberEdit->text().trimmed().toStdString() };
        const auto localIdText{ m_localIdText.toStdString() };

        if (imoText.empty() && localIdText.empty())
        {
            m_universalIdValue->setText(QString{});
            return;
        }

        const auto result{ core::buildUniversalId(imoText, localIdText) };

        if (!result.universalId.has_value())
        {
            m_universalIdValue->setText(tr("(invalid)"));

            QStringList lines;
            for (const auto& error : result.errors)
            {
                lines << detail::toQString(error.type) + QStringLiteral(": ") + detail::toQString(error.message);
            }
            m_errorsValue->setText(lines.join(QStringLiteral("\n")));
            return;
        }

        m_universalIdText = detail::toQString(*result.universalId);
        m_universalIdValue->setText(m_universalIdText);
        m_copyButton->setEnabled(true);
    }
} // namespace qvistools::gui
