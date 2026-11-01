#include "LocalIdBuilderWidget.h"

#include "core/vis/gmod/NodeCategory.h"

#include "app/gui/detail/QStringUtils.h"
#include "GmodPathItemSlot.h"

#include <dnv/vista/sdk/core/GmodPath.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

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

            slot.setChips(std::move(segments));
        }
    } // namespace

    LocalIdBuilderWidget::LocalIdBuilderWidget(dnv::vista::sdk::VisVersion version, QWidget* parent)
        : QGroupBox{ tr("Local ID builder"), parent },
          m_versionLabel{ makeSeparatorLabel(versionLabelText(version), this) },
          m_primaryItemSlot{ new GmodPathItemSlot{ tr("Primary"), this } },
          m_secondaryItemSlot{ new GmodPathItemSlot{ tr("Secondary"), this } }
    {
        setupLayout();
        setupConnections();
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
        m_versionLabel->setText(versionLabelText(version));

        m_primaryItemSlot->clear();
        m_secondaryItemSlot->clear();
    }

    void LocalIdBuilderWidget::setupLayout()
    {
        auto* layout{ new QHBoxLayout{ this } };
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(4);

        layout->addWidget(m_versionLabel);
        layout->addWidget(m_primaryItemSlot);
        layout->addWidget(makeSeparatorLabel(QStringLiteral("/ sec /"), this));
        layout->addWidget(m_secondaryItemSlot);
        layout->addWidget(makeSeparatorLabel(QStringLiteral("/ meta"), this));
        layout->addStretch(1);
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
} // namespace qvistools::gui
