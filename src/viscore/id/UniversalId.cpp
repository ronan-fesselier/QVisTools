#include "UniversalId.h"

#include <dnv/vista/sdk/core/ImoNumber.h>
#include <dnv/vista/sdk/core/UniversalId.h>
#include <dnv/vista/sdk/core/UniversalIdBuilder.h>

#include <exception>

namespace viscore::id
{
    UniversalIdBuildResult buildUniversalId(
        dnv::vista::sdk::VisVersion version,
        const std::string& imoNumberText,
        const dnv::vista::sdk::LocalIdBuilder& localIdBuilder)
    {
        const auto imoNumber{ dnv::vista::sdk::ImoNumber::fromString(imoNumberText) };
        if (!imoNumber.has_value())
        {
            return { std::nullopt,
                     { { UniversalIdBuildErrorType::InvalidImoNumber,
                         "'" + imoNumberText + "' is not a valid IMO number" } } };
        }

        try
        {
            const auto universalId{ dnv::vista::sdk::UniversalIdBuilder::create(version)
                                        .withImoNumber(*imoNumber)
                                        .withLocalId(localIdBuilder)
                                        .build() };
            return { universalId.toString(), {} };
        }
        catch (const std::exception& e)
        {
            return { std::nullopt, { { UniversalIdBuildErrorType::ValidationError, e.what() } } };
        }
    }
} // namespace viscore::id
