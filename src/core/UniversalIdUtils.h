#pragma once

#include <optional>
#include <string>
#include <vector>

namespace qvistools::core
{
    struct UniversalIdBuildError
    {
        std::string type;
        std::string message;
    };

    struct UniversalIdBuildResult
    {
        std::optional<std::string> universalId;
        std::vector<UniversalIdBuildError> errors;
    };

    [[nodiscard]] UniversalIdBuildResult buildUniversalId(
        const std::string& imoNumberText, const std::string& localIdText);
} // namespace qvistools::core
