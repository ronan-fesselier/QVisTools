#include "LocalIdConversionDialog.h"

#include "app/detail/QStringUtils.h"

#include <viscore/id/LocalId.h>

#include <dnv/vista/sdk/core/VIS.h>

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace qvistools
{
    LocalIdConversionDialog::LocalIdConversionDialog(dnv::vista::sdk::LocalIdBuilder sourceBuilder, QWidget* parent)
        : QDialog{ parent },
          m_sourceBuilder{ std::move(sourceBuilder) },
          m_targetVersionCombo{ new QComboBox{ this } },
          m_beforeValue{ new QLabel{ this } },
          m_afterValue{ new QLabel{ this } },
          m_errorValue{ new QLabel{ this } }
    {
        setWindowTitle(tr("Convert to another VIS version"));
        setSizeGripEnabled(false);

        const auto sourceVersion{ *m_sourceBuilder.version() };

        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        for (const auto version : vis.versions())
        {
            if (version <= sourceVersion)
            {
                continue;
            }
            m_targetVersionCombo->addItem(
                detail::toQString(dnv::vista::sdk::VisVersions::toString(version)),
                QVariant::fromValue(static_cast<int>(version)));
        }

        m_beforeValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
        m_beforeValue->setText(detail::toQString(m_sourceBuilder.toString()));

        m_afterValue->setTextInteractionFlags(Qt::TextSelectableByMouse);

        m_errorValue->setWordWrap(true);
        m_errorValue->setVisible(false);

        auto* form{ new QFormLayout };
        form->addRow(tr("Target version"), m_targetVersionCombo);
        form->addRow(tr("Before"), m_beforeValue);
        form->addRow(tr("After"), m_afterValue);

        auto* buttonBox{ new QDialogButtonBox{ QDialogButtonBox::Cancel, this } };
        auto* convertButton{ buttonBox->addButton(tr("Convert"), QDialogButtonBox::AcceptRole) };
        convertButton->setDefault(true);

        QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

        auto* layout{ new QVBoxLayout{ this } };
        layout->addLayout(form);
        layout->addWidget(m_errorValue);
        layout->addWidget(buttonBox);

        QObject::connect(
            m_targetVersionCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &LocalIdConversionDialog::updatePreview);

        updatePreview();
    }

    void LocalIdConversionDialog::updatePreview()
    {
        m_resultBuilder.reset();
        m_afterValue->clear();

        const auto index{ m_targetVersionCombo->currentIndex() };
        if (index < 0)
        {
            m_errorValue->setText(tr("No later VIS version is available to convert to"));
            m_errorValue->setVisible(true);
            return;
        }

        const auto targetVersion{ static_cast<dnv::vista::sdk::VisVersion>(
            m_targetVersionCombo->itemData(index).toInt()) };

        const auto result{ viscore::id::convertLocalId(m_sourceBuilder, targetVersion) };
        if (!result.builder.has_value())
        {
            QStringList lines;
            for (const auto& error : result.errors)
            {
                lines << detail::toQString(error.message);
            }
            m_errorValue->setText(lines.join(QStringLiteral("\n")));
            m_errorValue->setVisible(true);
            setFixedSize(sizeHint());
            return;
        }

        m_errorValue->setVisible(false);
        m_resultBuilder = result.builder;
        m_afterValue->setText(detail::toQString(result.builder->toString()));
        setFixedSize(sizeHint());
    }

    void LocalIdConversionDialog::accept()
    {
        if (!m_resultBuilder.has_value())
        {
            return;
        }
        QDialog::accept();
    }

    const dnv::vista::sdk::LocalIdBuilder& LocalIdConversionDialog::result() const noexcept
    {
        return *m_resultBuilder;
    }
} // namespace qvistools
