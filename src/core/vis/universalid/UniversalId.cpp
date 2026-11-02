#include "UniversalId.h"

#include "core/vis/localid/LocalId.h"

#include <dnv/vista/sdk/core/ImoNumber.h>
#include <dnv/vista/sdk/core/UniversalId.h>
#include <dnv/vista/sdk/core/UniversalIdBuilder.h>

#include <exception>
#include <utility>

namespace qvistools::core::vis
{
    UniversalIdBuildResult buildUniversalId(
        dnv::vista::sdk::VisVersion version,
        const std::string& imoNumberText,
        const std::optional<dnv::vista::sdk::GmodPath>& primaryItem,
        const std::optional<dnv::vista::sdk::GmodPath>& secondaryItem,
        const std::vector<std::pair<dnv::vista::sdk::CodebookName, std::string>>& tags,
        bool verbose)
    {
        const auto imoNumber{ dnv::vista::sdk::ImoNumber::fromString(imoNumberText) };
        if (!imoNumber.has_value())
        {
            return { std::nullopt,
                     { { UniversalIdBuildErrorType::InvalidImoNumber,
                         "'" + imoNumberText + "' is not a valid IMO number" } } };
        }

        auto builderResult{ buildLocalIdBuilder(version, primaryItem, secondaryItem, tags, verbose) };
        if (!builderResult.builder.has_value())
        {
            std::vector<UniversalIdBuildError> errors;
            for (const auto& error : builderResult.errors)
            {
                const auto type{ error.type == LocalIdBuildErrorType::InvalidTag
                                     ? UniversalIdBuildErrorType::InvalidTag
                                     : UniversalIdBuildErrorType::ValidationError };
                errors.push_back({ type, error.message });
            }
            return { std::nullopt, std::move(errors) };
        }

        try
        {
            const auto universalId{ dnv::vista::sdk::UniversalIdBuilder::create(version)
                                        .withImoNumber(*imoNumber)
                                        .withLocalId(*builderResult.builder)
                                        .build() };
            return { universalId.toString(), {} };
        }
        catch (const std::exception& e)
        {
            return { std::nullopt, { { UniversalIdBuildErrorType::ValidationError, e.what() } } };
        }
    }
} // namespace qvistools::core::vis
