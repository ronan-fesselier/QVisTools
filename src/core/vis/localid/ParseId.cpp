#include "ParseId.h"

#include <dnv/vista/sdk/core/ImoNumber.h>
#include <dnv/vista/sdk/core/LocalId.h>
#include <dnv/vista/sdk/core/LocalIdBuilder.h>
#include <dnv/vista/sdk/core/MetadataTag.h>
#include <dnv/vista/sdk/core/ParsingErrors.h>
#include <dnv/vista/sdk/core/UniversalId.h>
#include <dnv/vista/sdk/core/UniversalIdBuilder.h>

namespace qvistools::core::vis
{
    namespace
    {
        ParsedId toParsedId(const dnv::vista::sdk::LocalIdBuilder& builder, std::optional<std::string> imoNumberText)
        {
            ParsedId parsed;
            parsed.version = *builder.version();
            parsed.primaryItem = builder.primaryItem();
            parsed.secondaryItem = builder.secondaryItem();
            parsed.verbose = builder.isVerboseMode();
            parsed.imoNumberText = std::move(imoNumberText);

            for (const auto& tag : builder.metadataTags())
            {
                parsed.tags.emplace_back(tag.name(), tag.value());
            }

            return parsed;
        }

        std::vector<std::string> toErrorMessages(const dnv::vista::sdk::ParsingErrors& errors)
        {
            std::vector<std::string> messages;
            for (const auto& error : errors)
            {
                messages.push_back(error.message);
            }
            return messages;
        }
    } // namespace

    ParseIdResult parseExistingId(const std::string& text)
    {
        const bool isUniversalId{ text.find(dnv::vista::sdk::UniversalIdBuilder::namingEntity()) != std::string::npos };

        if (isUniversalId)
        {
            dnv::vista::sdk::ParsingErrors errors;
            const auto universalId{ dnv::vista::sdk::UniversalIdBuilder::fromString(text, errors) };
            if (!universalId.has_value())
            {
                return { std::nullopt, toErrorMessages(errors) };
            }

            auto parsed{ toParsedId(universalId->localId().builder(), universalId->imoNumber().toString()) };
            return { std::move(parsed), {} };
        }

        dnv::vista::sdk::ParsingErrors errors;
        const auto localId{ dnv::vista::sdk::LocalIdBuilder::fromString(text, errors) };
        if (!localId.has_value())
        {
            return { std::nullopt, toErrorMessages(errors) };
        }

        auto parsed{ toParsedId(localId->builder(), std::nullopt) };
        return { std::move(parsed), {} };
    }
} // namespace qvistools::core::vis
