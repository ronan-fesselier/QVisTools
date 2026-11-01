#pragma once

#include <QStyledItemDelegate>

class QPainter;
class QStyleOptionViewItem;

namespace qvistools
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
    };
} // namespace qvistools
