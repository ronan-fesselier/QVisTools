#pragma once

#include <dnv/vista/sdk/core/CodebookName.h>
#include <dnv/vista/sdk/core/GmodPath.h>
#include <dnv/vista/sdk/core/VisVersions.h>

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace viscore::id
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

    [[nodiscard]] ParseIdResult parseExistingId(std::string_view text);
} // namespace viscore::id
