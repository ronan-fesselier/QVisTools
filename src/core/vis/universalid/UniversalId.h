#pragma once

#include <dnv/vista/sdk/core/CodebookName.h>
#include <dnv/vista/sdk/core/VisVersions.h>

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace dnv::vista::sdk
{
    class GmodPath;
} // namespace dnv::vista::sdk

namespace qvistools::core::vis
{
    enum class UniversalIdBuildErrorType
    {
        InvalidImoNumber = 0,
        InvalidTag,
        ValidationError
    };

    struct UniversalIdBuildError
    {
        UniversalIdBuildErrorType type;
        std::string message;
    };

    struct UniversalIdBuildResult
    {
        std::optional<std::string> universalId;
        std::vector<UniversalIdBuildError> errors;
    };

    /**
     * @brief Build a UniversalId string from an IMO number, an optional primary item,
     *        an optional secondary item, and a set of metadata tags
     */
    [[nodiscard]] UniversalIdBuildResult buildUniversalId(
        dnv::vista::sdk::VisVersion version,
        const std::string& imoNumberText,
        const std::optional<dnv::vista::sdk::GmodPath>& primaryItem,
        const std::optional<dnv::vista::sdk::GmodPath>& secondaryItem,
        const std::vector<std::pair<dnv::vista::sdk::CodebookName, std::string>>& tags,
        bool verbose);
} // namespace qvistools::core::vis
