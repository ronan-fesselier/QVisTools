#include "LocalIdBuilderUtils.h"

#include <dnv/vista/sdk/core/Codebooks.h>
#include <dnv/vista/sdk/core/GmodPath.h>
#include <dnv/vista/sdk/core/LocalId.h>
#include <dnv/vista/sdk/core/LocalIdBuilder.h>
#include <dnv/vista/sdk/core/VIS.h>

#include <stdexcept>
#include <utility>

namespace qvistools::core
{
    LocalIdBuildResult buildLocalId(
        const dnv::vista::sdk::GmodPath& primaryItem,
        const std::optional<dnv::vista::sdk::GmodPath>& secondaryItem,
        const std::vector<std::pair<dnv::vista::sdk::CodebookName, std::string>>& tags,
        bool verbose)
    {
        const auto& vis{ dnv::vista::sdk::VIS::instance() };
        const auto& codebooks{ vis.codebooks(primaryItem.version()) };

        auto builder{ dnv::vista::sdk::LocalIdBuilder::create(primaryItem.version())
                           .withPrimaryItem(primaryItem)
                           .withVerboseMode(verbose) };

        if (secondaryItem.has_value())
        {
            builder = std::move(builder).withSecondaryItem(*secondaryItem);
        }

        for (const auto& [name, text] : tags)
        {
            const auto tag{ codebooks[name].createTag(text) };
            if (!tag.has_value())
            {
                return { std::nullopt,
                         { { "InvalidTag",
                             "The value '" + text + "' is not valid for codebook " +
                                 std::string{ dnv::vista::sdk::CodebookNames::toString(name) } } } };
            }
            builder = std::move(builder).withMetadataTag(*tag);
        }

        try
        {
            const auto localId{ builder.build() };
            return { localId.toString(), {} };
        }
        catch (const std::invalid_argument& e)
        {
            return { std::nullopt, { { "ValidationError", e.what() } } };
        }
    }
} // namespace qvistools::core
