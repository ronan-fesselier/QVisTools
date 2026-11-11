#pragma once

#include <QColor>
#include <QFontMetrics>
#include <QPoint>
#include <QString>

#include <vector>

class QPainter;

namespace dnv::vista::sdk
{
    class GmodNode;
} // namespace dnv::vista::sdk

namespace qvistools::gui::chip
{
    constexpr int Height{ 18 };
    constexpr int VerticalMargin{ 4 };
    constexpr int GroupSpacing{ 6 };

    struct Segment
    {
        QString text;
        QColor color;
    };

    [[nodiscard]] QColor codeChipColor(const dnv::vista::sdk::GmodNode& node);

    [[nodiscard]] int segmentWidth(const QFontMetrics& metrics, const Segment& segment);

    int drawChipGroup(QPainter* painter, QPoint topLeft, const std::vector<Segment>& segments);
} // namespace qvistools::gui::chip
