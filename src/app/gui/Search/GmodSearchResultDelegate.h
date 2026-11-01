#pragma once

#include "app/gui/Shared/GmodChipRenderer.h"

#include <QModelIndex>
#include <QPoint>
#include <QStyledItemDelegate>

#include <vector>

class QAbstractItemView;
class QHelpEvent;
class QPainter;
class QRect;
class QStyleOptionViewItem;

namespace qvistools::gui
{
    class GmodSearchResultDelegate final : public QStyledItemDelegate
    {
        Q_OBJECT

    public:
        enum Role
        {
            NodeRole = Qt::UserRole,
            ParentRole
        };

        explicit GmodSearchResultDelegate(QObject* parent = nullptr);

        virtual void paint(
            QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        virtual bool helpEvent(
            QHelpEvent* event,
            QAbstractItemView* view,
            const QStyleOptionViewItem& option,
            const QModelIndex& index) override;

    private:
        static std::vector<ChipSegment> buildSegments(const QModelIndex& index);
        static QPoint chipTopLeft(const QRect& rect);
    };
} // namespace qvistools::gui
