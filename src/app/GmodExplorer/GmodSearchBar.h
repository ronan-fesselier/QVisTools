#pragma once

#include <viscore/gmod/SearchIndex.h>

#include <QWidget>

#include <memory>
#include <vector>

class QLineEdit;
class QListWidget;
class QTimer;

namespace dnv::vista::sdk
{
    class Gmod;
} // namespace dnv::vista::sdk

namespace qvistools
{
    class GmodSearchBar final : public QWidget
    {
        Q_OBJECT

    public:
        explicit GmodSearchBar(const dnv::vista::sdk::Gmod& gmod, QWidget* parent = nullptr);
        virtual ~GmodSearchBar() override;

        void setGmod(const dnv::vista::sdk::Gmod& gmod);

    private:
        void setupLayout();
        void setupConnections();

        void closeResults();
        void activateCurrentItem();

        void onTextChanged(const QString& text);
        void onItemActivated();

        const viscore::gmod::SearchIndex& index();

        const dnv::vista::sdk::Gmod* m_gmod;
        std::unique_ptr<viscore::gmod::SearchIndex> m_index;
        std::vector<viscore::gmod::SearchIndex::Match> m_currentMatches;

        QLineEdit* m_lineEdit;
        QListWidget* m_resultsList;
        QTimer* m_debounceTimer;

    protected:
        virtual bool eventFilter(QObject* watched, QEvent* event) override;

    signals:
        void matchActivated(const viscore::gmod::SearchIndex::Match& match);
    };
} // namespace qvistools
