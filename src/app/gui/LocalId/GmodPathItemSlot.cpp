#include "GmodPathItemSlot.h"

#include "app/gui/LocalId/GmodPathIndividualizeDialog.h"

#include <dnv/vista/sdk/core/GmodNode.h>

#include <QAction>
#include <QFontMetrics>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

namespace qvistools::gui
{
    namespace
    {
        constexpr int PLACEHOLDER_PADDING_H{ 8 };
    } // namespace

    GmodPathItemSlot::GmodPathItemSlot(QString placeholderText, QWidget* parent)
        : QAbstractButton{ parent },
          m_placeholderText{ std::move(placeholderText) }
    {
        unsetCursor();
    }

    void GmodPathItemSlot::setChips(dnv::vista::sdk::GmodPath path, std::vector<ChipSegment> segments)
    {
        m_path = std::move(path);
        m_chips = std::move(segments);
        setCursor(Qt::PointingHandCursor);
        updateGeometry();
        update();
    }

    void GmodPathItemSlot::clear()
    {
        m_path.reset();
        m_chips.clear();
        unsetCursor();
        updateGeometry();
        update();
    }

    void GmodPathItemSlot::mousePressEvent(QMouseEvent* event)
    {
        if (event->button() != Qt::LeftButton || !isFilled())
        {
            event->ignore();
            return;
        }

        auto* menu{ new QMenu{ this } };

        if (m_path.has_value() && m_path->isIndividualizable())
        {
            auto* editLocationAction{ menu->addAction(tr("Edit location")) };
            QObject::connect(editLocationAction, &QAction::triggered, this, [this]() {
                GmodPathIndividualizeDialog dialog{ *m_path, std::nullopt, this };
                if (dialog.exec() == QDialog::Accepted)
                {
                    emit pathEdited(dialog.result());
                }
            });
        }

        auto* unmapAction{ menu->addAction(tr("Unmap")) };
        QObject::connect(unmapAction, &QAction::triggered, this, &GmodPathItemSlot::unmapRequested);

        menu->popup(event->globalPosition().toPoint());
    }

    void GmodPathItemSlot::paintEvent(QPaintEvent* /*event*/)
    {
        QPainter painter{ this };
        painter.setRenderHint(QPainter::Antialiasing);

        const QRect bounds{ rect() };
        const QPoint topLeft{ bounds.left(), bounds.top() + (bounds.height() - CHIP_HEIGHT) / 2 };

        if (!m_chips.empty())
        {
            drawChipGroup(&painter, topLeft, m_chips);
            return;
        }

        const QRect placeholderRect{ topLeft, QSize{ bounds.width(), CHIP_HEIGHT } };

        QPainterPath outline;
        outline.addRoundedRect(placeholderRect, CHIP_RADIUS, CHIP_RADIUS);

        const QColor accent{ palette().color(QPalette::PlaceholderText) };
        painter.setPen(QPen{ accent, 1, Qt::DashLine });
        painter.drawPath(outline);

        painter.setPen(accent);
        painter.drawText(placeholderRect, Qt::AlignCenter, m_placeholderText);
    }

    QSize GmodPathItemSlot::sizeHint() const
    {
        const QFontMetrics metrics{ font() };

        if (m_chips.empty())
        {
            return { metrics.horizontalAdvance(m_placeholderText) + 2 * PLACEHOLDER_PADDING_H, CHIP_HEIGHT };
        }

        int chipsWidth{ 0 };
        for (const auto& segment : m_chips)
        {
            chipsWidth += segmentWidth(metrics, segment);
        }

        return { chipsWidth, CHIP_HEIGHT };
    }

} // namespace qvistools::gui
