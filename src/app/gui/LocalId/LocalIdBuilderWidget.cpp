#include "LocalIdBuilderWidget.h"

#include "core/vis/codebooks/CodebookValues.h"
#include "core/vis/gmod/NodeCategory.h"
#include "core/vis/localid/LocalId.h"
#include "core/vis/universalid/UniversalId.h"

#include "app/gui/detail/QStringUtils.h"
#include "GmodPathItemSlot.h"
#include "MetadataTagRow.h"

#include <dnv/vista/sdk/core/Codebooks.h>
#include <dnv/vista/sdk/core/VIS.h>

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>

#include <string>
#include <utility>
#include <vector>

namespace qvistools::gui
{
    namespace
    {
        QLabel* makeSeparatorLabel(const QString& text, QWidget* parent)
        {
            auto* label{ new QLabel{ text, parent } };
            label->setStyleSheet(QStringLiteral("color: palette(window-text);"));
            return label;
        }

        QString versionLabelText(dnv::vista::sdk::VisVersion version)
        {
            return QStringLiteral("/ dnv-v2 / vis-%1 /")
                .arg(detail::toQString(dnv::vista::sdk::VisVersions::toString(version)));
        }

        void fillSlot(GmodPathItemSlot& slot, const std::optional<dnv::vista::sdk::GmodPath>& path)
        {
            if (!path.has_value())
            {
                slot.clear();
                return;
            }

            const std::string shortPath{ path->toString() };

            std::vector<std::string> codes;
            std::size_t start{ 0 };
            for (std::size_t i{ 0 }; i <= shortPath.size(); ++i)
            {
                if (i == shortPath.size() || shortPath[i] == '/')
                {
                    if (i > start)
                    {
                        codes.emplace_back(shortPath.substr(start, i - start));
                    }
                    start = i + 1;
                }
            }

            std::vector<ChipSegment> segments;
            for (const auto& code : codes)
            {
                const dnv::vista::sdk::GmodNode* foundNode{ nullptr };
                for (std::size_t i{ 0 }; i < path->length(); ++i)
                {
                    const auto& node{ (*path)[i] };
                    const auto nodeCode{ node.toString() };
                    if (code == nodeCode || code.rfind(nodeCode + "-", 0) == 0)
                    {
                        foundNode = &node;
                        break;
                    }
                }

                const auto category{ foundNode ? core::vis::categoryOf(*foundNode) : core::vis::NodeCategory::Other };
                const bool isMappable{ foundNode != nullptr && foundNode->isMappable() };
                const auto icon{ isMappable ? ChipIcon::Unlink : ChipIcon::None };
                segments.push_back({ QString::fromStdString(code), categoryColor(category), category, icon });
            }

            slot.setChips(*path, std::move(segments));
        }
    } // namespace

    LocalIdBuilderWidget::LocalIdBuilderWidget(dnv::vista::sdk::VisVersion version, QWidget* parent)
        : QGroupBox{ tr("Local ID and Universal ID builder"), parent },
          m_version{ version },
          m_versionLabel{ makeSeparatorLabel(versionLabelText(version), this) },
          m_primaryItemSlot{ new GmodPathItemSlot{ tr("Primary"), this } },
          m_secondaryItemSlot{ new GmodPathItemSlot{ tr("Secondary"), this } },
          m_metadataRows{},
          m_verboseCheckBox{ new QCheckBox{ tr("Verbose"), this } },
          m_localIdValue{ new QLineEdit{ this } },
          m_copyButton{ new QPushButton{ tr("Copy"), this } },
          m_imoEdit{ new QLineEdit{ this } },
          m_universalIdValue{ new QLineEdit{ this } },
          m_universalIdCopyButton{ new QPushButton{ tr("Copy"), this } },
          m_localIdErrorsValue{ new QLabel{ this } },
          m_universalIdErrorsValue{ new QLabel{ this } }
    {
        setupLayout();
        setupConnections();
        rebuildMetadataRows();
    }

    void LocalIdBuilderWidget::setPrimaryItem(const std::optional<dnv::vista::sdk::GmodPath>& path)
    {
        m_primaryPath = path;
        fillSlot(*m_primaryItemSlot, path);
        rebuildLocalId();
    }

    void LocalIdBuilderWidget::setSecondaryItem(const std::optional<dnv::vista::sdk::GmodPath>& path)
    {
        m_secondaryPath = path;
        fillSlot(*m_secondaryItemSlot, path);
        rebuildLocalId();
    }

    void LocalIdBuilderWidget::setVisVersion(dnv::vista::sdk::VisVersion version)
    {
        m_version = version;
        m_versionLabel->setText(versionLabelText(version));

        m_primaryPath.reset();
        m_secondaryPath.reset();
        m_primaryItemSlot->clear();
        m_secondaryItemSlot->clear();

        rebuildMetadataRows();
    }

    void LocalIdBuilderWidget::setupLayout()
    {
        auto* grid{ new QGridLayout{ this } };
        grid->setContentsMargins(0, 0, 0, 0);
        grid->setSpacing(4);

        int col = 0;
        grid->addWidget(m_versionLabel, 1, col++);
        grid->addWidget(m_primaryItemSlot, 1, col++);
        grid->addWidget(makeSeparatorLabel(QStringLiteral("/ sec /"), this), 1, col++);
        grid->addWidget(m_secondaryItemSlot, 1, col++);
        grid->addWidget(makeSeparatorLabel(QStringLiteral("/ meta"), this), 1, col++);

        m_metadataColumn = col;

        grid->setColumnStretch(m_metadataColumn, 1);

        const auto resultRow{ static_cast<int>(core::vis::DISPLAYED_CODEBOOKS.size()) + 1 };

        m_localIdValue->setReadOnly(true);

        auto* localIdLayout{ new QHBoxLayout };
        localIdLayout->addWidget(m_localIdValue, 1);
        localIdLayout->addWidget(m_verboseCheckBox);
        localIdLayout->addWidget(m_copyButton);
        grid->addLayout(localIdLayout, resultRow, 0, 1, m_metadataColumn + 1);

        m_imoEdit->setPlaceholderText(tr("IMO number"));
        m_imoEdit->setMaximumWidth(120);

        m_universalIdValue->setReadOnly(true);

        auto* universalIdLayout{ new QHBoxLayout };
        universalIdLayout->addWidget(makeSeparatorLabel(tr("IMO"), this));
        universalIdLayout->addWidget(m_imoEdit);
        universalIdLayout->addWidget(m_universalIdValue, 1);
        universalIdLayout->addWidget(m_universalIdCopyButton);
        grid->addLayout(universalIdLayout, resultRow + 1, 0, 1, m_metadataColumn + 1);

        QPalette errorPalette{ m_localIdErrorsValue->palette() };
        errorPalette.setColor(QPalette::WindowText, Qt::red);

        m_localIdErrorsValue->setWordWrap(true);
        m_localIdErrorsValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
        m_localIdErrorsValue->setPalette(errorPalette);
        grid->addWidget(m_localIdErrorsValue, resultRow + 2, 0, 1, m_metadataColumn + 1);

        m_universalIdErrorsValue->setWordWrap(true);
        m_universalIdErrorsValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
        m_universalIdErrorsValue->setPalette(errorPalette);
        grid->addWidget(m_universalIdErrorsValue, resultRow + 3, 0, 1, m_metadataColumn + 1);

        grid->setRowStretch(resultRow + 4, 1);
    }

    void LocalIdBuilderWidget::setupConnections()
    {
        QObject::connect(
            m_primaryItemSlot,
            &GmodPathItemSlot::unmapRequested,
            this,
            &LocalIdBuilderWidget::primaryItemUnmapRequested);
        QObject::connect(
            m_secondaryItemSlot,
            &GmodPathItemSlot::unmapRequested,
            this,
            &LocalIdBuilderWidget::secondaryItemUnmapRequested);
        QObject::connect(m_verboseCheckBox, &QCheckBox::toggled, this, &LocalIdBuilderWidget::rebuildLocalId);

        QObject::connect(
            m_copyButton, &QPushButton::clicked, this, [this]() { QApplication::clipboard()->setText(m_localIdText); });

        QObject::connect(
            m_primaryItemSlot, &GmodPathItemSlot::pathEdited, this, [this](const dnv::vista::sdk::GmodPath& path) {
                setPrimaryItem(path);
            });
        QObject::connect(
            m_secondaryItemSlot, &GmodPathItemSlot::pathEdited, this, [this](const dnv::vista::sdk::GmodPath& path) {
                setSecondaryItem(path);
            });

        QObject::connect(m_imoEdit, &QLineEdit::textChanged, this, &LocalIdBuilderWidget::rebuildLocalId);

        QObject::connect(m_universalIdCopyButton, &QPushButton::clicked, this, [this]() {
            QApplication::clipboard()->setText(m_universalIdText);
        });
    }

    void LocalIdBuilderWidget::rebuildMetadataRows()
    {
        auto* grid{ qobject_cast<QGridLayout*>(layout()) };

        for (auto& row : m_metadataRows)
        {
            if (row != nullptr)
            {
                grid->removeWidget(row);
                row->hide();
                row->deleteLater();
                row = nullptr;
            }
        }

        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& codebooks{ vis.codebooks(m_version) };

        for (std::size_t i{ 0 }; i < core::vis::DISPLAYED_CODEBOOKS.size(); ++i)
        {
            const auto name{ core::vis::DISPLAYED_CODEBOOKS[i] };
            const auto standardValues{ core::vis::standardValuesOf(codebooks, name) };

            auto* row{ new MetadataTagRow{ name, standardValues, this } };
            m_metadataRows[i] = row;
            grid->addWidget(row, static_cast<int>(i) + 1, m_metadataColumn);

            QObject::connect(row, &MetadataTagRow::valueChanged, this, [this](auto, auto) { rebuildLocalId(); });
        }

        rebuildLocalId();
    }

    void LocalIdBuilderWidget::rebuildLocalId()
    {
        std::vector<std::pair<dnv::vista::sdk::CodebookName, std::string>> tags;
        for (const auto* row : m_metadataRows)
        {
            if (row == nullptr)
            {
                continue;
            }

            const auto text{ row->value().trimmed().toStdString() };
            if (!text.empty())
            {
                tags.emplace_back(row->codebookName(), text);
            }
        }

        const auto result{ core::vis::buildLocalId(
            m_version, m_primaryPath, m_secondaryPath, tags, m_verboseCheckBox->isChecked()) };

        m_localIdText.clear();
        m_localIdErrorsValue->clear();

        if (!result.localId.has_value())
        {
            m_localIdValue->setText(tr("(invalid)"));
            m_copyButton->setEnabled(false);

            QStringList lines;
            for (const auto& error : result.errors)
            {
                lines << detail::toQString(error.message);
            }
            m_localIdErrorsValue->setText(lines.join(QStringLiteral("\n")));
        }
        else
        {
            m_localIdText = detail::toQString(*result.localId);
            m_localIdValue->setText(m_localIdText);
            m_copyButton->setEnabled(true);
        }

        emit localIdChanged(m_localIdText);

        m_universalIdText.clear();
        m_universalIdValue->clear();
        m_universalIdCopyButton->setEnabled(false);
        m_universalIdErrorsValue->clear();

        const auto imoText{ m_imoEdit->text().trimmed().toStdString() };
        if (imoText.empty())
        {
            return;
        }

        const auto universalIdResult{ core::vis::buildUniversalId(
            m_version, imoText, m_primaryPath, m_secondaryPath, tags, m_verboseCheckBox->isChecked()) };

        if (!universalIdResult.universalId.has_value())
        {
            m_universalIdValue->setText(tr("(invalid)"));

            QStringList lines;
            for (const auto& error : universalIdResult.errors)
            {
                lines << detail::toQString(error.message);
            }
            m_universalIdErrorsValue->setText(lines.join(QStringLiteral("\n")));
            return;
        }

        m_universalIdText = detail::toQString(*universalIdResult.universalId);
        m_universalIdValue->setText(m_universalIdText);
        m_universalIdCopyButton->setEnabled(true);
    }
} // namespace qvistools::gui
