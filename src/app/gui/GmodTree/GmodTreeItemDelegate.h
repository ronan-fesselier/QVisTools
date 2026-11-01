#pragma once

#include "app/gui/Shared/GmodChipRenderer.h"

#include <QModelIndex>
#include <QPoint>
#include <QSize>
#include <QStyledItemDelegate>

#include <vector>

class QAbstractItemView;
class QHelpEvent;
class QPainter;
class QRect;
class QStyleOptionViewItem;

namespace qvistools::gui
{
    class GmodTreeItemDelegate final : public QStyledItemDelegate
    {
        Q_OBJECT

    public:
        explicit GmodTreeItemDelegate(QObject* parent = nullptr);

        virtual void paint(
            QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        virtual bool helpEvent(
            QHelpEvent* event,
            QAbstractItemView* view,
            const QStyleOptionViewItem& option,
            const QModelIndex& index) override;

        static std::vector<ChipSegment> buildSegments(const QModelIndex& index);
        static QPoint chipTopLeft(const QRect& rect);
    };
} // namespace qvistools::gui
