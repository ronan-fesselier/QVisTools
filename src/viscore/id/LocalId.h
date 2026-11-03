#pragma once

#include <dnv/vista/sdk/core/CodebookName.h>
#include <dnv/vista/sdk/core/LocalIdBuilder.h>
#include <dnv/vista/sdk/core/VisVersions.h>

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace dnv::vista::sdk
{
    class GmodPath;
    class LocalId;
} // namespace dnv::vista::sdk

namespace viscore::id
{
    enum class LocalIdBuildErrorType
    {
        InvalidTag = 0,
        ValidationError
    };

    struct LocalIdBuildError
    {
        LocalIdBuildErrorType type;
        std::string message;
    };

    struct LocalIdBuildResult
    {
        std::optional<std::string> localId;
        std::vector<LocalIdBuildError> errors;
    };

    struct LocalIdBuilderResult
    {
        std::optional<dnv::vista::sdk::LocalIdBuilder> builder;
        std::vector<LocalIdBuildError> errors;
    };

    [[nodiscard]] LocalIdBuilderResult buildLocalIdBuilder(
        dnv::vista::sdk::VisVersion version,
        const std::optional<dnv::vista::sdk::GmodPath>& primaryItem,
        const std::optional<dnv::vista::sdk::GmodPath>& secondaryItem,
        const std::vector<std::pair<dnv::vista::sdk::CodebookName, std::string>>& tags,
        bool verbose);

    [[nodiscard]] LocalIdBuildResult buildLocalId(
        dnv::vista::sdk::VisVersion version,
        const std::optional<dnv::vista::sdk::GmodPath>& primaryItem,
        const std::optional<dnv::vista::sdk::GmodPath>& secondaryItem,
        const std::vector<std::pair<dnv::vista::sdk::CodebookName, std::string>>& tags,
        bool verbose);
} // namespace viscore::id
