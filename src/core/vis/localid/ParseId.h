#pragma once

#include <dnv/vista/sdk/core/CodebookName.h>
#include <dnv/vista/sdk/core/GmodPath.h>
#include <dnv/vista/sdk/core/VisVersions.h>

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace qvistools::core::vis
{
    struct ParsedId
    {
        dnv::vista::sdk::VisVersion version;
        std::optional<dnv::vista::sdk::GmodPath> primaryItem;
        std::optional<dnv::vista::sdk::GmodPath> secondaryItem;
        std::vector<std::pair<dnv::vista::sdk::CodebookName, std::string>> tags;
        bool verbose;
        std::optional<std::string> imoNumberText;
    };

    struct ParseIdResult
    {
        std::optional<ParsedId> id;
        std::vector<std::string> errors;
    };

    /**
     * @brief Parse an existing Local ID or Universal ID string (as produced by
     *        buildLocalId()/buildUniversalId()) back into its components, so the
     *        builder UI can be repopulated from a pasted ID
     */
    [[nodiscard]] ParseIdResult parseExistingId(const std::string& text);
} // namespace qvistools::core::vis
