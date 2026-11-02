#include "GmodPathIndividualizeDialog.h"

#include "core/vis/gmod/GmodPathLocation.h"

#include "app/gui/detail/QStringUtils.h"

#include <dnv/vista/sdk/core/GmodNode.h>
#include <dnv/vista/sdk/core/LocationBuilder.h>
#include <dnv/vista/sdk/core/Locations.h>
#include <dnv/vista/sdk/core/VIS.h>

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

namespace qvistools::gui
{
    namespace
    {
        void populateGroupCombo(
            QComboBox* combo, const dnv::vista::sdk::Locations& locations, dnv::vista::sdk::LocationGroup group)
        {
            combo->addItem(QObject::tr("(none)"), QVariant{});

            const auto groupsIt{ locations.groups().find(group) };
            if (groupsIt == locations.groups().end())
            {
                return;
            }

            for (const auto& relativeLocation : groupsIt->second)
            {
                combo->addItem(
                    detail::toQString(relativeLocation.name()), QVariant::fromValue(QChar{ relativeLocation.code() }));
            }
        }
    } // namespace

    GmodPathIndividualizeDialog::GmodPathIndividualizeDialog(
        dnv::vista::sdk::GmodPath path, std::optional<std::size_t> onlySetIndex, QWidget* parent)
        : QDialog{ parent },
          m_sourcePath{ path },
          m_resultPath{ std::move(path) },
          m_previewValue{ new QLabel{ this } },
          m_errorValue{ new QLabel{ this } }
    {
        setWindowTitle(tr("Map path nodes"));

        auto* cardsLayout{ new QVBoxLayout };

        const auto sets{ m_sourcePath.individualizableSets() };
        for (std::size_t setIndex{ 0 }; setIndex < sets.size(); ++setIndex)
        {
            if (onlySetIndex.has_value() && setIndex != *onlySetIndex)
            {
                continue;
            }
            setupSetCard(cardsLayout, setIndex);
        }

        m_previewValue->setWordWrap(true);

        m_errorValue->setWordWrap(true);
        m_errorValue->setVisible(false);

        auto* previewBox{ new QGroupBox{ tr("Will be mapped as"), this } };
        auto* previewLayout{ new QVBoxLayout{ previewBox } };
        previewLayout->addWidget(m_previewValue);

        auto* buttonBox{ new QDialogButtonBox{ QDialogButtonBox::Cancel, this } };
        auto* mapButton{ buttonBox->addButton(tr("Map"), QDialogButtonBox::AcceptRole) };
        mapButton->setDefault(true);

        QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

        auto* layout{ new QVBoxLayout{ this } };
        layout->addLayout(cardsLayout);
        layout->addWidget(previewBox);
        layout->addWidget(m_errorValue);
        layout->addWidget(buttonBox);

        updatePreview();
    }

    void GmodPathIndividualizeDialog::setupSetCard(QVBoxLayout* cardsLayout, std::size_t setIndex)
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& locations{ vis.locations(m_sourcePath.version()) };

        const auto sets{ m_sourcePath.individualizableSets() };
        const auto& set{ sets[setIndex] };

        QStringList nodeNames;
        for (const auto& node : set.nodes())
        {
            nodeNames << detail::toQString(node.code());
        }

        auto* card{ new QGroupBox{ nodeNames.join(QStringLiteral(" / ")), this } };
        auto* cardLayout{ new QVBoxLayout{ card } };

        const auto existingLocation{ set.location() };

        auto* numberLabel{ new QLabel{ tr("Number"), card } };
        auto* numberEdit{ new QSpinBox{ card } };
        numberEdit->setRange(0, 99);
        numberEdit->setSpecialValueText(tr("(none)"));

        auto* sideLabel{ new QLabel{ tr("Side"), card } };
        auto* sideCombo{ new QComboBox{ card } };
        populateGroupCombo(sideCombo, locations, dnv::vista::sdk::LocationGroup::Side);

        auto* verticalLabel{ new QLabel{ tr("Vertical"), card } };
        auto* verticalCombo{ new QComboBox{ card } };
        populateGroupCombo(verticalCombo, locations, dnv::vista::sdk::LocationGroup::Vertical);

        auto* transverseLabel{ new QLabel{ tr("Transverse"), card } };
        auto* transverseCombo{ new QComboBox{ card } };
        populateGroupCombo(transverseCombo, locations, dnv::vista::sdk::LocationGroup::Transverse);

        auto* longitudinalLabel{ new QLabel{ tr("Longitudinal"), card } };
        auto* longitudinalCombo{ new QComboBox{ card } };
        populateGroupCombo(longitudinalCombo, locations, dnv::vista::sdk::LocationGroup::Longitudinal);

        if (existingLocation.has_value())
        {
            for (const char c : existingLocation->value())
            {
                if (c >= '0' && c <= '9')
                {
                    numberEdit->setValue(numberEdit->value() * 10 + (c - '0'));
                    continue;
                }

                for (auto* combo : { sideCombo, verticalCombo, transverseCombo, longitudinalCombo })
                {
                    const auto index{ combo->findData(QVariant::fromValue(QChar{ c })) };
                    if (index >= 0)
                    {
                        combo->setCurrentIndex(index);
                        break;
                    }
                }
            }
        }

        cardLayout->addWidget(numberLabel);
        cardLayout->addWidget(numberEdit);
        cardLayout->addWidget(sideLabel);
        cardLayout->addWidget(sideCombo);
        cardLayout->addWidget(verticalLabel);
        cardLayout->addWidget(verticalCombo);
        cardLayout->addWidget(transverseLabel);
        cardLayout->addWidget(transverseCombo);
        cardLayout->addWidget(longitudinalLabel);
        cardLayout->addWidget(longitudinalCombo);

        cardsLayout->addWidget(card);

        m_cards.push_back({ setIndex, numberEdit, sideCombo, verticalCombo, transverseCombo, longitudinalCombo });

        QObject::connect(
            numberEdit, QOverload<int>::of(&QSpinBox::valueChanged), this, &GmodPathIndividualizeDialog::updatePreview);
        QObject::connect(
            sideCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &GmodPathIndividualizeDialog::updatePreview);
        QObject::connect(
            verticalCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &GmodPathIndividualizeDialog::updatePreview);
        QObject::connect(
            transverseCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &GmodPathIndividualizeDialog::updatePreview);
        QObject::connect(
            longitudinalCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &GmodPathIndividualizeDialog::updatePreview);
    }

    void GmodPathIndividualizeDialog::updatePreview()
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& locations{ vis.locations(m_sourcePath.version()) };

        auto path{ m_sourcePath };
        QString error;

        for (const auto& card : m_cards)
        {
            const auto setIndex{ card.setIndex };

            const bool hasNumber{ card.numberEdit->value() > 0 };
            const auto sideCode{ card.sideCombo->currentData() };
            const auto verticalCode{ card.verticalCombo->currentData() };
            const auto transverseCode{ card.transverseCombo->currentData() };
            const auto longitudinalCode{ card.longitudinalCombo->currentData() };

            if (!hasNumber && !sideCode.isValid() && !verticalCode.isValid() && !transverseCode.isValid() &&
                !longitudinalCode.isValid())
            {
                continue;
            }

            auto builder{ dnv::vista::sdk::LocationBuilder::create(locations) };

            if (hasNumber)
            {
                builder = std::move(builder).withNumber(card.numberEdit->value());
            }
            if (sideCode.isValid())
            {
                builder = std::move(builder).withSide(sideCode.toChar().toLatin1());
            }
            if (verticalCode.isValid())
            {
                builder = std::move(builder).withVertical(verticalCode.toChar().toLatin1());
            }
            if (transverseCode.isValid())
            {
                builder = std::move(builder).withTransverse(transverseCode.toChar().toLatin1());
            }
            if (longitudinalCode.isValid())
            {
                builder = std::move(builder).withLongitudinal(longitudinalCode.toChar().toLatin1());
            }

            const auto location{ builder.build() };
            const auto result{ core::vis::applyLocationToSet(path, setIndex, location) };

            if (!result.path.has_value())
            {
                error = detail::toQString(result.error->message);
                break;
            }
            path = *result.path;
        }

        if (!error.isEmpty())
        {
            m_errorValue->setText(error);
            m_errorValue->setVisible(true);
            m_previewValue->setText(QString{});
            return;
        }

        m_errorValue->setVisible(false);
        m_resultPath = path;
        m_previewValue->setText(detail::toQString(m_resultPath.toString()));
    }

    void GmodPathIndividualizeDialog::accept()
    {
        if (m_errorValue->isVisible())
        {
            return;
        }
        QDialog::accept();
    }

    const dnv::vista::sdk::GmodPath& GmodPathIndividualizeDialog::result() const noexcept
    {
        return m_resultPath;
    }
} // namespace qvistools::gui
