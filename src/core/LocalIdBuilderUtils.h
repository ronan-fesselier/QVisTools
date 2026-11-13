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
    class LocalId;
} // namespace dnv::vista::sdk

namespace qvistools::core
{
    struct LocalIdBuildError
    {
        std::string type;
        std::string message;
    };

    struct LocalIdBuildResult
    {
        std::optional<std::string> localId;
        std::vector<LocalIdBuildError> errors;
    };

    [[nodiscard]] LocalIdBuildResult buildLocalId(
        const dnv::vista::sdk::GmodPath& primaryItem,
        const std::optional<dnv::vista::sdk::GmodPath>& secondaryItem,
        const std::vector<std::pair<dnv::vista::sdk::CodebookName, std::string>>& tags,
        bool verbose);

    [[nodiscard]] std::optional<dnv::vista::sdk::LocalId> convertLocalId(
        const dnv::vista::sdk::LocalId& localId, dnv::vista::sdk::VisVersion targetVersion);
} // namespace qvistools::core
