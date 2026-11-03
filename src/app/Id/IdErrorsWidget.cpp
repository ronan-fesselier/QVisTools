#include "IdErrorsWidget.h"

#include <QFontMetrics>
#include <QLabel>
#include <QPalette>
#include <QScrollArea>
#include <QVBoxLayout>

namespace qvistools
{
    namespace
    {
        constexpr int VISIBLE_ERROR_LINES{ 5 };
    } // namespace

    IdErrorsWidget::IdErrorsWidget(QWidget* parent)
        : QGroupBox{ tr("Errors"), parent },
          m_errorsLabel{ new QLabel{ this } }
    {
        m_errorsLabel->setWordWrap(true);
        m_errorsLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        m_errorsLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        QPalette errorPalette{ m_errorsLabel->palette() };
        errorPalette.setColor(QPalette::WindowText, Qt::red);
        m_errorsLabel->setPalette(errorPalette);

        auto* scrollArea{ new QScrollArea{ this } };
        scrollArea->setWidget(m_errorsLabel);
        scrollArea->setWidgetResizable(true);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setFixedHeight(
            QFontMetrics{ m_errorsLabel->font() }.lineSpacing() * VISIBLE_ERROR_LINES + 2 * scrollArea->frameWidth());

        auto* layout{ new QVBoxLayout{ this } };
        layout->addWidget(scrollArea);
    }

    void IdErrorsWidget::setParseErrors(const QStringList& errors)
    {
        m_parseErrors = errors;
        refresh();
    }

    void IdErrorsWidget::setLocalIdErrors(const QStringList& errors)
    {
        m_localIdErrors = errors;
        refresh();
    }

    void IdErrorsWidget::setUniversalIdErrors(const QStringList& errors)
    {
        m_universalIdErrors = errors;
        refresh();
    }

    void IdErrorsWidget::refresh()
    {
        QStringList all{ m_parseErrors };
        all << m_localIdErrors;
        all << m_universalIdErrors;
        m_errorsLabel->setText(all.join(QStringLiteral("\n")));
    }
} // namespace qvistools
