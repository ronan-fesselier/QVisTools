#include "LocalId.h"

#include <dnv/vista/sdk/core/Codebooks.h>
#include <dnv/vista/sdk/core/GmodPath.h>
#include <dnv/vista/sdk/core/LocalId.h>
#include <dnv/vista/sdk/core/LocalIdBuilder.h>
#include <dnv/vista/sdk/core/VIS.h>

#include <exception>
#include <utility>

namespace qvistools::core::vis
{
    LocalIdBuildResult buildLocalId(
        dnv::vista::sdk::VisVersion version,
        const std::optional<dnv::vista::sdk::GmodPath>& primaryItem,
        const std::optional<dnv::vista::sdk::GmodPath>& secondaryItem,
        const std::vector<std::pair<dnv::vista::sdk::CodebookName, std::string>>& tags,
        bool verbose)
    {
        const auto& codebooks{ dnv::vista::sdk::VIS::instance().codebooks(version) };

        auto builder{ dnv::vista::sdk::LocalIdBuilder::create(version).withVerboseMode(verbose) };

        if (primaryItem.has_value())
        {
            builder = std::move(builder).withPrimaryItem(*primaryItem);
        }

        if (secondaryItem.has_value())
        {
            builder = std::move(builder).withSecondaryItem(*secondaryItem);
        }

        std::vector<LocalIdBuildError> errors;
        for (const auto& [name, text] : tags)
        {
            const auto tag{ codebooks[name].createTag(text) };
            if (!tag.has_value())
            {
                errors.push_back({ LocalIdBuildErrorType::InvalidTag,
                                   "The value '" + text + "' is not valid for codebook " +
                                       std::string{ dnv::vista::sdk::CodebookNames::toString(name) } });
                continue;
            }
            builder = std::move(builder).withMetadataTag(*tag);
        }

        if (!errors.empty())
        {
            return { std::nullopt, std::move(errors) };
        }

        try
        {
            const auto localId{ builder.build() };
            return { localId.toString(), {} };
        }
        catch (const std::exception& e)
        {
            return { std::nullopt, { { LocalIdBuildErrorType::ValidationError, e.what() } } };
        }
    }
} // namespace qvistools::core::vis
