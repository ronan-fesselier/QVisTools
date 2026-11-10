#include "GmodChipRenderer.h"

#include <dnv/vista/sdk/core/GmodNode.h>

#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>

namespace qvistools::gui::chip
{
    namespace
    {
        constexpr int Radius{ 4 };
        constexpr int PaddingH{ 6 };

        QColor textColorFor(const QColor& background)
        {
            const double yiq{ (background.red() * 299 + background.green() * 587 + background.blue() * 114) / 1000.0 };
            return yiq >= 128.0 ? Qt::black : Qt::white;
        }
    } // namespace

    QColor codeChipColor(const dnv::vista::sdk::GmodNode& node)
    {
        if (node.isRoot())
        {
            return QColor{ "#004d00" };
        }
        if (node.isLeafNode())
        {
            return node.isAssetFunctionNode() ? QColor{ "#00ff00" } : QColor{ "#ccffcc" };
        }
        if (node.isProductType())
        {
            return QColor{ "#ff0000" };
        }
        if (node.isFunctionComposition())
        {
            return QColor{ "#99cc00" };
        }

        if (node.metadata().type() == "GROUP")
        {
            return QColor{ "#008000" };
        }
        if (node.metadata().type() == "SELECTION")
        {
            return QColor{ "#ff9100" };
        }

        return QColor{ "#9b59b6" }; // Unhandled node category (visible marker)
    }

    int drawChipGroup(QPainter* painter, QPoint topLeft, const std::vector<Segment>& segments)
    {
        const QFontMetrics metrics{ painter->fontMetrics() };

        std::vector<int> widths;
        widths.reserve(segments.size());
        int totalWidth{ 0 };
        for (const auto& segment : segments)
        {
            const int width{ metrics.horizontalAdvance(segment.text) + 2 * PaddingH };
            widths.push_back(width);
            totalWidth += width;
        }

        const QRect groupRect{ topLeft, QSize{ totalWidth, Height } };

        QPainterPath outline;
        outline.addRoundedRect(groupRect, Radius, Radius);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setClipPath(outline);

        int x{ topLeft.x() };
        for (std::size_t i{ 0 }; i < segments.size(); ++i)
        {
            const QRect segmentRect{ QPoint{ x, topLeft.y() }, QSize{ widths[i], Height } };
            painter->fillRect(segmentRect, segments[i].color);
            painter->setPen(textColorFor(segments[i].color));
            painter->drawText(segmentRect, Qt::AlignCenter, segments[i].text);
            x += widths[i];
        }

        painter->setClipping(false);
        painter->setPen(QPen{ QColor{ 0, 0, 0, 60 } });
        x = topLeft.x();
        for (std::size_t i{ 0 }; i + 1 < segments.size(); ++i)
        {
            x += widths[i];
            painter->drawLine(QPoint{ x, topLeft.y() }, QPoint{ x, topLeft.y() + Height });
        }

        painter->restore();

        return totalWidth;
    }
} // namespace qvistools::gui::chip
