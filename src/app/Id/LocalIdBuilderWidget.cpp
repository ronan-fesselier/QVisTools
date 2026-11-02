#include "LocalIdBuilderWidget.h"

#include "app/detail/QStringUtils.h"
#include "GmodPathItemSlot.h"
#include "MetadataTagRow.h"

#include <viscore/codebooks/CodebookValues.h>
#include <viscore/gmod/NodeCategory.h>

#include <dnv/vista/sdk/core/VIS.h>

#include <QFontMetrics>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

namespace qvistools
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
                slot.setToolTip({});
                return;
            }

            const auto& metadata{ path->node().metadata() };
            const auto name{ metadata.commonName().has_value() ? std::string_view(*metadata.commonName())
                                                               : metadata.name() };
            slot.setToolTip(detail::toQString(name));

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

                const auto category{ foundNode ? viscore::gmod::categoryOf(*foundNode)
                                               : viscore::gmod::NodeCategory::Other };
                const bool isMappable{ foundNode != nullptr && foundNode->isMappable() };
                const auto icon{ isMappable ? ChipIcon::Unlink : ChipIcon::None };
                segments.push_back({ QString::fromStdString(code), categoryColor(category), category, icon });
            }

            slot.setChips(std::move(segments));
        }
    } // namespace

    LocalIdBuilderWidget::LocalIdBuilderWidget(dnv::vista::sdk::VisVersion version, QWidget* parent)
        : QGroupBox{ tr("Local ID builder"), parent },
          m_version{ version },
          m_versionLabel{ makeSeparatorLabel(versionLabelText(version), this) },
          m_primaryItemSlot{ new GmodPathItemSlot{ tr("Primary"), this } },
          m_secondaryItemSlot{ new GmodPathItemSlot{ tr("Secondary"), this } },
          m_metadataRows{}
    {
        setupLayout();
        setupConnections();
        rebuildMetadataRows();
    }

    void LocalIdBuilderWidget::setPrimaryItem(const std::optional<dnv::vista::sdk::GmodPath>& path)
    {
        fillSlot(*m_primaryItemSlot, path);
    }

    void LocalIdBuilderWidget::setSecondaryItem(const std::optional<dnv::vista::sdk::GmodPath>& path)
    {
        fillSlot(*m_secondaryItemSlot, path);
    }

    void LocalIdBuilderWidget::setVisVersion(dnv::vista::sdk::VisVersion version)
    {
        m_version = version;
        m_versionLabel->setText(versionLabelText(version));

        m_primaryItemSlot->clear();
        m_secondaryItemSlot->clear();

        rebuildMetadataRows();
    }

    void LocalIdBuilderWidget::setupLayout()
    {
        auto* grid{ new QGridLayout{ this } };

        int col = 0;
        grid->addWidget(m_versionLabel, 0, col++);
        grid->addWidget(m_primaryItemSlot, 0, col++);
        grid->addWidget(makeSeparatorLabel(QStringLiteral("/ sec /"), this), 0, col++);
        grid->addWidget(m_secondaryItemSlot, 0, col++);
        grid->addWidget(makeSeparatorLabel(QStringLiteral("/ meta"), this), 0, col++);

        m_metadataColumn = col;

        grid->setColumnStretch(m_metadataColumn, 1);
        grid->setRowStretch(static_cast<int>(viscore::codebooks::DISPLAYED_CODEBOOKS.size()), 1);
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

        std::array<std::vector<std::string>, viscore::codebooks::DISPLAYED_CODEBOOKS.size()> standardValuesPerCodebook;

        const QFontMetrics metrics{ font() };
        int maxValueWidth{ 0 };

        for (std::size_t i{ 0 }; i < viscore::codebooks::DISPLAYED_CODEBOOKS.size(); ++i)
        {
            standardValuesPerCodebook[i] =
                viscore::codebooks::standardValuesOf(codebooks, viscore::codebooks::DISPLAYED_CODEBOOKS[i]);

            for (const auto& value : standardValuesPerCodebook[i])
            {
                maxValueWidth = std::max(maxValueWidth, metrics.horizontalAdvance(detail::toQString(value)));
            }
        }

        constexpr int VALUE_FIELD_PADDING{ 32 };

        for (std::size_t i{ 0 }; i < viscore::codebooks::DISPLAYED_CODEBOOKS.size(); ++i)
        {
            auto* row{ new MetadataTagRow{
                viscore::codebooks::DISPLAYED_CODEBOOKS[i], standardValuesPerCodebook[i], this } };
            row->setValueFieldMinimumWidth(maxValueWidth + VALUE_FIELD_PADDING);

            m_metadataRows[i] = row;
            grid->addWidget(row, static_cast<int>(i), m_metadataColumn);
        }
    }
} // namespace qvistools
