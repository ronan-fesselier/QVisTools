#pragma once

#include <QModelIndex>
#include <QSize>
#include <QStyledItemDelegate>

class QHelpEvent;
class QPainter;
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
    };
} // namespace qvistools::gui
