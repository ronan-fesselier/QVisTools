#pragma once

#include <dnv/vista/sdk/core/LocalIdBuilder.h>
#include <dnv/vista/sdk/core/VisVersions.h>

#include <optional>
#include <string>
#include <vector>

namespace viscore::id
{
    enum class UniversalIdBuildErrorType
    {
        InvalidImoNumber = 0,
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

    [[nodiscard]] UniversalIdBuildResult buildUniversalId(
        dnv::vista::sdk::VisVersion version,
        const std::string& imoNumberText,
        const dnv::vista::sdk::LocalIdBuilder& localIdBuilder);
} // namespace viscore::id
