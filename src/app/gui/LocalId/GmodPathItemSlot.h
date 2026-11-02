#pragma once

#include "app/gui/Shared/GmodChipRenderer.h"

#include <dnv/vista/sdk/core/GmodPath.h>

#include <QAbstractButton>

#include <optional>
#include <vector>

namespace qvistools::gui
{
    /**
     * @brief A slot in the Local ID path bar, showing either a "Primary"/"Secondary"
     *        placeholder (empty state) or the selected GmodPath's short path as a row
     *        of category-colored chips, one per leaf segment (filled state, e.g.
     *        "411.1" / "C101.31")
     */
    class GmodPathItemSlot final : public QAbstractButton
    {
        Q_OBJECT

    public:
        explicit GmodPathItemSlot(QString placeholderText, QWidget* parent = nullptr);

        /**
         * @brief Set the filled state, showing one chip per segment
         */
        void setChips(dnv::vista::sdk::GmodPath path, std::vector<ChipSegment> segments);

        /**
         * @brief Clear back to the empty placeholder state
         */
        void clear();

        [[nodiscard]] bool isFilled() const noexcept { return !m_chips.empty(); }

    protected:
        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual void paintEvent(QPaintEvent* event) override;
        virtual QSize sizeHint() const override;

    private:
        QString m_placeholderText;
        std::vector<ChipSegment> m_chips;
        std::optional<dnv::vista::sdk::GmodPath> m_path;

    signals:
        void unmapRequested();
        void pathEdited(const dnv::vista::sdk::GmodPath& path);
    };
} // namespace qvistools::gui
