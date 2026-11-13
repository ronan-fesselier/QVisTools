#include "LocalIdBuilderWidget.h"

#include "core/LocalIdBuilderUtils.h"
#include "detail/StringUtils.h"

#include <dnv/vista/sdk/core/CodebookName.h>
#include <dnv/vista/sdk/core/Codebooks.h>
#include <dnv/vista/sdk/core/GmodPath.h>
#include <dnv/vista/sdk/core/LocalId.h>
#include <dnv/vista/sdk/core/VIS.h>

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QCompleter>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPalette>
#include <QPushButton>
#include <QStringList>
#include <QToolButton>
#include <QVBoxLayout>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

namespace qvistools::gui
{
    namespace
    {
        constexpr std::array<dnv::vista::sdk::CodebookName, LocalIdBuilderWidget::TagCount> TagCodebooks{
            dnv::vista::sdk::CodebookName::Quantity,    dnv::vista::sdk::CodebookName::Content,
            dnv::vista::sdk::CodebookName::Calculation, dnv::vista::sdk::CodebookName::State,
            dnv::vista::sdk::CodebookName::Command,     dnv::vista::sdk::CodebookName::Type,
            dnv::vista::sdk::CodebookName::Position,    dnv::vista::sdk::CodebookName::Detail,
        };
    } // namespace

    LocalIdBuilderWidget::LocalIdBuilderWidget(QWidget* parent)
        : QWidget{ parent },
          m_primaryItemValue{ new QLabel{ this } },
          m_setPrimaryItemButton{ new QPushButton{ tr("Set from tree"), this } },
          m_secondaryItemValue{ new QLabel{ this } },
          m_setSecondaryItemButton{ new QPushButton{ tr("Set from tree"), this } },
          m_clearSecondaryItemButton{ new QPushButton{ tr("Clear"), this } },
          m_verboseCheckBox{ new QCheckBox{ tr("Verbose"), this } },
          m_localIdValue{ new QLineEdit{ this } },
          m_copyButton{ new QPushButton{ tr("Copy"), this } },
          m_convertButton{ new QToolButton{ this } },
          m_convertMenu{ new QMenu{ this } },
          m_errorsValue{ new QLabel{ this } }
    {
        for (auto& combo : m_tagCombos)
        {
            combo = new QComboBox{ this };
            combo->setEditable(true);
            combo->setInsertPolicy(QComboBox::NoInsert);
            combo->setFixedWidth(200);

            auto* completer{ new QCompleter{ combo->model(), combo } };
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            completer->setFilterMode(Qt::MatchContains);
            completer->setCompletionMode(QCompleter::PopupCompletion);
            combo->setCompleter(completer);
        }

        m_copyButton->setEnabled(false);
        m_clearSecondaryItemButton->setEnabled(false);

        m_convertButton->setText(tr("Convert to..."));
        m_convertButton->setPopupMode(QToolButton::InstantPopup);
        m_convertButton->setMenu(m_convertMenu);
        m_convertButton->setEnabled(false);

        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        for (const auto version : vis.versions())
        {
            auto* action{ m_convertMenu->addAction(
                detail::toQString(dnv::vista::sdk::VisVersions::toString(version))) };
            QObject::connect(action, &QAction::triggered, this, [this, version]() { emit convertRequested(version); });
            m_convertActions.emplace_back(version, action);
        }

        setupLayout();
        setupConnections();

        setPath(std::nullopt);
    }

    void LocalIdBuilderWidget::setPath(const std::optional<dnv::vista::sdk::GmodPath>& path)
    {
        m_primaryItem = path;

        m_primaryItemValue->setText(
            m_primaryItem.has_value() ? detail::toQString(m_primaryItem->toString()) : QString{});

        setSecondaryItemPath(std::nullopt);
        updateConvertMenu();

        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& codebooks{ vis.codebooks(m_primaryItem.has_value() ? m_primaryItem->version() : vis.latest()) };

        for (int i{ 0 }; i < TagCount; ++i)
        {
            auto* combo{ m_tagCombos[static_cast<std::size_t>(i)] };

            std::vector<std::string> values;
            for (const auto& value : codebooks[TagCodebooks[static_cast<std::size_t>(i)]].standardValues())
            {
                values.emplace_back(value);
            }
            std::stable_sort(values.begin(), values.end());

            const QSignalBlocker blocker{ combo };
            combo->clear();
            combo->addItem(QString{});
            for (const auto& value : values)
            {
                combo->addItem(detail::toQString(value));
            }
        }

        rebuildLocalId();
    }

    void LocalIdBuilderWidget::setupLayout()
    {
        auto* formLayout{ new QFormLayout };

        auto* primaryItemLayout{ new QHBoxLayout };
        primaryItemLayout->addWidget(m_primaryItemValue);
        primaryItemLayout->addWidget(m_setPrimaryItemButton);
        primaryItemLayout->addStretch(1);
        formLayout->addRow(tr("Primary item:"), primaryItemLayout);

        auto* secondaryItemLayout{ new QHBoxLayout };
        secondaryItemLayout->addWidget(m_secondaryItemValue);
        secondaryItemLayout->addWidget(m_setSecondaryItemButton);
        secondaryItemLayout->addWidget(m_clearSecondaryItemButton);
        secondaryItemLayout->addStretch(1);
        formLayout->addRow(tr("Secondary item:"), secondaryItemLayout);

        for (int i{ 0 }; i < TagCount; ++i)
        {
            const auto name{ dnv::vista::sdk::CodebookNames::toString(TagCodebooks[static_cast<std::size_t>(i)]) };
            formLayout->addRow(detail::toQString(name) + ":", m_tagCombos[static_cast<std::size_t>(i)]);
        }

        m_localIdValue->setReadOnly(true);

        auto* localIdLayout{ new QHBoxLayout };
        localIdLayout->addWidget(m_localIdValue, 1);
        localIdLayout->addWidget(m_verboseCheckBox);
        localIdLayout->addWidget(m_copyButton);
        localIdLayout->addWidget(m_convertButton);
        formLayout->addRow(tr("LocalId:"), localIdLayout);

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

    void LocalIdBuilderWidget::setupConnections()
    {
        QObject::connect(
            m_setPrimaryItemButton, &QPushButton::clicked, this, [this]() { emit primaryItemRequested(); });

        QObject::connect(
            m_setSecondaryItemButton, &QPushButton::clicked, this, [this]() { emit secondaryItemRequested(); });

        QObject::connect(
            m_clearSecondaryItemButton, &QPushButton::clicked, this, [this]() { setSecondaryItemPath(std::nullopt); });

        for (auto* combo : m_tagCombos)
        {
            QObject::connect(combo, &QComboBox::currentTextChanged, this, &LocalIdBuilderWidget::rebuildLocalId);
        }

        QObject::connect(m_verboseCheckBox, &QCheckBox::toggled, this, &LocalIdBuilderWidget::rebuildLocalId);

        QObject::connect(
            m_copyButton, &QPushButton::clicked, this, [this]() { QApplication::clipboard()->setText(m_localIdText); });
    }

    void LocalIdBuilderWidget::updateConvertMenu()
    {
        for (const auto& [version, action] : m_convertActions)
        {
            action->setEnabled(
                m_primaryItem.has_value() && static_cast<int>(version) > static_cast<int>(m_primaryItem->version()));
        }
    }

    void LocalIdBuilderWidget::setSecondaryItemPath(const std::optional<dnv::vista::sdk::GmodPath>& path)
    {
        m_secondaryItem = path;

        m_secondaryItemValue->setText(
            m_secondaryItem.has_value() ? detail::toQString(m_secondaryItem->toString()) : QString{});
        m_clearSecondaryItemButton->setEnabled(m_secondaryItem.has_value());

        rebuildLocalId();
    }

    void LocalIdBuilderWidget::setLocalId(const dnv::vista::sdk::LocalId& localId)
    {
        m_suppressRebuild = true;

        setPath(localId.primaryItem());
        setSecondaryItemPath(localId.secondaryItem());

        m_verboseCheckBox->setChecked(localId.isVerboseMode());

        for (const auto& tag : localId.metadataTags())
        {
            const auto it{ std::find(TagCodebooks.begin(), TagCodebooks.end(), tag.name()) };
            if (it == TagCodebooks.end())
            {
                continue;
            }

            const auto index{ static_cast<std::size_t>(std::distance(TagCodebooks.begin(), it)) };
            m_tagCombos[index]->setCurrentText(detail::toQString(tag.value()));
        }

        m_suppressRebuild = false;
        rebuildLocalId();
    }

    void LocalIdBuilderWidget::setParsingError(const QString& message)
    {
        m_localIdValue->setText(tr("(invalid)"));
        m_errorsValue->setText(message);
        m_localIdText.clear();
        m_copyButton->setEnabled(false);
        m_convertButton->setEnabled(false);
    }

    void LocalIdBuilderWidget::rebuildLocalId()
    {
        if (m_suppressRebuild)
        {
            return;
        }

        m_errorsValue->clear();
        m_localIdText.clear();
        m_copyButton->setEnabled(false);
        m_convertButton->setEnabled(false);

        if (!m_primaryItem.has_value())
        {
            m_localIdValue->setText(QString{});
            return;
        }

        std::vector<std::pair<dnv::vista::sdk::CodebookName, std::string>> tags;
        for (int i{ 0 }; i < TagCount; ++i)
        {
            const auto text{ m_tagCombos[static_cast<std::size_t>(i)]->currentText().trimmed().toStdString() };
            if (!text.empty())
            {
                tags.emplace_back(TagCodebooks[static_cast<std::size_t>(i)], text);
            }
        }

        if (tags.empty())
        {
            m_localIdValue->setText(tr("(incomplete - select at least one metadata tag)"));
            return;
        }

        const auto result{ core::buildLocalId(*m_primaryItem, m_secondaryItem, tags, m_verboseCheckBox->isChecked()) };

        if (!result.localId.has_value())
        {
            m_localIdValue->setText(tr("(invalid)"));

            QStringList lines;
            for (const auto& error : result.errors)
            {
                lines << detail::toQString(error.type) + QStringLiteral(": ") + detail::toQString(error.message);
            }
            m_errorsValue->setText(lines.join(QStringLiteral("\n")));
            return;
        }

        m_localIdText = detail::toQString(*result.localId);
        m_localIdValue->setText(m_localIdText);
        m_copyButton->setEnabled(true);
        m_convertButton->setEnabled(true);
    }
} // namespace qvistools::gui
