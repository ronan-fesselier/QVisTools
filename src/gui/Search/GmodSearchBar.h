#pragma once

#include <QWidget>

#include <memory>

class QLineEdit;
class QListWidget;

namespace dnv::vista::sdk
{
    class Gmod;
    class GmodNode;
} // namespace dnv::vista::sdk

namespace qvistools::core
{
    class GmodSearchIndex;
} // namespace qvistools::core

namespace qvistools::gui
{
    class GmodSearchBar final : public QWidget
    {
        Q_OBJECT

    public:
        explicit GmodSearchBar(const dnv::vista::sdk::Gmod& gmod, QWidget* parent = nullptr);
        ~GmodSearchBar() override;

        void setGmod(const dnv::vista::sdk::Gmod& gmod);

    protected:
        virtual bool eventFilter(QObject* watched, QEvent* event) override;

    private:
        void setupLayout();
        void setupConnections();

        void closeResults();
        void activateCurrentItem();

        void onTextChanged(const QString& text);
        void onItemActivated();

        const core::GmodSearchIndex& index();

        const dnv::vista::sdk::Gmod* m_gmod;
        std::unique_ptr<core::GmodSearchIndex> m_index;

        QLineEdit* m_lineEdit;
        QListWidget* m_resultsList;

    signals:
        void nodeActivated(const dnv::vista::sdk::GmodNode& node);
    };
} // namespace qvistools::gui
