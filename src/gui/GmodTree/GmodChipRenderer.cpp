#include "GmodChipRenderer.h"

#include <dnv/vista/sdk/core/GmodNode.h>

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

    Category categoryOf(const dnv::vista::sdk::GmodNode& node)
    {
        if (node.isRoot())
        {
            return Category::Root;
        }
        if (node.isLeafNode())
        {
            return node.isAssetFunctionNode() ? Category::LeafAssetFunction : Category::Leaf;
        }
        if (node.isProductType())
        {
            return Category::ProductType;
        }
        if (node.isFunctionComposition())
        {
            return Category::FunctionComposition;
        }
        if (node.metadata().type() == "GROUP")
        {
            const auto cat = node.metadata().category();
            if (cat == "ASSET FUNCTION")
            {
                return Category::AssetFunctionGroup;
            }
            if (cat == "PRODUCT FUNCTION")
            {
                return Category::ProductFunctionGroup;
            }
            return Category::Unhandled;
        }
        if (node.metadata().type() == "SELECTION")
        {
            const auto cat = node.metadata().category();
            if (cat == "ASSET FUNCTION" || cat == "PRODUCT FUNCTION")
            {
                return Category::FunctionSelection;
            }
            if (cat == "PRODUCT")
            {
                return Category::ProductSelection;
            }
            return Category::Unhandled;
        }

        return Category::Unhandled;
    }

    QColor categoryColor(Category category)
    {
        switch (category)
        {
            case Category::Root:
                return QColor{ "#004d00" };
            case Category::LeafAssetFunction:
                return QColor{ "#00ff00" };
            case Category::Leaf:
                return QColor{ "#ccffcc" };
            case Category::ProductType:
                return QColor{ "#ff0000" };
            case Category::FunctionComposition:
                return QColor{ "#99cc00" };
            case Category::AssetFunctionGroup:
            case Category::ProductFunctionGroup:
                return QColor{ "#008000" };
            case Category::FunctionSelection:
            case Category::ProductSelection:
                return QColor{ "#ff9100" };
            case Category::Unhandled:
                break;
        }
        return QColor{ "#9b59b6" }; // Unhandled node category (visible marker)
    }

    QString categoryName(Category category)
    {
        switch (category)
        {
            case Category::Root:
                return QStringLiteral("Root");
            case Category::LeafAssetFunction:
                return QStringLiteral("Leaf - asset function");
            case Category::Leaf:
                return QStringLiteral("Leaf");
            case Category::ProductType:
                return QStringLiteral("Product type");
            case Category::FunctionComposition:
                return QStringLiteral("Function composition");
            case Category::AssetFunctionGroup:
                return QStringLiteral("Asset function group");
            case Category::ProductFunctionGroup:
                return QStringLiteral("Product function group");
            case Category::FunctionSelection:
                return QStringLiteral("Function selection");
            case Category::ProductSelection:
                return QStringLiteral("Product selection");
            case Category::Unhandled:
                break;
        }
        return QStringLiteral("Unhandled category");
    }

    int segmentWidth(const QFontMetrics& metrics, const Segment& segment)
    {
        return metrics.horizontalAdvance(segment.text) + 2 * PaddingH;
    }

    int drawChipGroup(QPainter* painter, QPoint topLeft, const std::vector<Segment>& segments)
    {
        const QFontMetrics metrics{ painter->fontMetrics() };

        std::vector<int> widths;
        widths.reserve(segments.size());
        int totalWidth{ 0 };
        for (const auto& segment : segments)
        {
            const int width{ segmentWidth(metrics, segment) };
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

    std::optional<std::size_t> hitTest(
        const QFontMetrics& metrics, QPoint topLeft, const std::vector<Segment>& segments, QPoint point)
    {
        if (point.y() < topLeft.y() || point.y() >= topLeft.y() + Height)
        {
            return std::nullopt;
        }

        int x{ topLeft.x() };
        for (std::size_t i{ 0 }; i < segments.size(); ++i)
        {
            const int width{ segmentWidth(metrics, segments[i]) };
            if (point.x() >= x && point.x() < x + width)
            {
                return i;
            }
            x += width;
        }

        return std::nullopt;
    }
} // namespace qvistools::gui::chip
