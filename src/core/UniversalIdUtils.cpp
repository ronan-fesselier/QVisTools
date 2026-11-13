#include "UniversalIdUtils.h"

#include <dnv/vista/sdk/core/ImoNumber.h>
#include <dnv/vista/sdk/core/LocalId.h>
#include <dnv/vista/sdk/core/UniversalId.h>
#include <dnv/vista/sdk/core/UniversalIdBuilder.h>

#include <stdexcept>

namespace qvistools::core
{
    UniversalIdBuildResult buildUniversalId(const std::string& imoNumberText, const std::string& localIdText)
    {
        if (imoNumberText.empty())
        {
            return { std::nullopt, { { "MissingImoNumber", "An IMO number is required" } } };
        }

        const auto imoNumber{ dnv::vista::sdk::ImoNumber::fromString(imoNumberText) };
        if (!imoNumber.has_value())
        {
            return { std::nullopt, { { "InvalidImoNumber", "'" + imoNumberText + "' is not a valid IMO number" } } };
        }

        if (localIdText.empty())
        {
            return { std::nullopt, { { "MissingLocalId", "A valid LocalId is required" } } };
        }

        const auto localId{ dnv::vista::sdk::LocalId::fromString(localIdText) };
        if (!localId.has_value())
        {
            return { std::nullopt, { { "InvalidLocalId", "'" + localIdText + "' is not a valid LocalId" } } };
        }

        try
        {
            const auto universalId{ dnv::vista::sdk::UniversalIdBuilder::create(localId->version())
                                        .withImoNumber(*imoNumber)
                                        .withLocalId(localId->builder())
                                        .build() };
            return { universalId.toString(), {} };
        }
        catch (const std::invalid_argument& e)
        {
            return { std::nullopt, { { "ValidationError", e.what() } } };
        }
    }
} // namespace qvistools::core
