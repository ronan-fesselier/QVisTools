#pragma once

#include "app/shared/GmodChipRenderer.h"

#include <QAbstractButton>

#include <vector>

namespace qvistools
{
    class GmodPathItemSlot final : public QAbstractButton
    {
        Q_OBJECT

    public:
        explicit GmodPathItemSlot(QString placeholderText, QWidget* parent = nullptr);

        void setChips(std::vector<ChipSegment> segments);
        void clear();

        [[nodiscard]] bool isFilled() const noexcept { return !m_chips.empty(); }

    protected:
        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual void paintEvent(QPaintEvent* event) override;
        virtual QSize sizeHint() const override;

    private:
        QString m_placeholderText;
        std::vector<ChipSegment> m_chips;

    signals:
        void unmapRequested();
    };
} // namespace qvistools
