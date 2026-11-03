#pragma once

#include <dnv/vista/sdk/core/LocalIdBuilder.h>
#include <dnv/vista/sdk/core/VisVersions.h>

#include <QGroupBox>
#include <QStringList>

#include <optional>

class QCheckBox;
class QLineEdit;
class QPushButton;

namespace qvistools
{
    class UniversalIdBuilderWidget final : public QGroupBox
    {
        Q_OBJECT

    public:
        explicit UniversalIdBuilderWidget(dnv::vista::sdk::VisVersion version, QWidget* parent = nullptr);

        void setVisVersion(dnv::vista::sdk::VisVersion version);
        void setLocalIdBuilder(const std::optional<dnv::vista::sdk::LocalIdBuilder>& localIdBuilder);
        void setImoNumberText(const QString& imoNumberText);
        void setVerbose(bool verbose);

    private:
        void setupLayout();
        void setupConnections();
        void rebuildUniversalId();

        dnv::vista::sdk::VisVersion m_version;
        std::optional<dnv::vista::sdk::LocalIdBuilder> m_localIdBuilder;

        QLineEdit* m_imoEdit;
        QCheckBox* m_verboseCheckBox;
        QLineEdit* m_universalIdValue;
        QPushButton* m_copyButton;
        QString m_universalIdText;

    signals:
        void errorsChanged(const QStringList& errors);
    };
} // namespace qvistools
