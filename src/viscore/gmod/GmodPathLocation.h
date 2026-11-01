#pragma once

#include <dnv/vista/sdk/core/GmodPath.h>

#include <cstddef>
#include <optional>
#include <string>

namespace dnv::vista::sdk
{
    class Location;
} // namespace dnv::vista::sdk

namespace viscore::gmod
{
    enum class ApplyLocationErrorType
    {
        OutOfRange = 0,
        ValidationError
    };

    struct ApplyLocationError
    {
        ApplyLocationErrorType type;
        std::string message;
    };

    struct ApplyLocationResult
    {
        std::optional<dnv::vista::sdk::GmodPath> path;
        std::optional<ApplyLocationError> error;
    };

    [[nodiscard]] ApplyLocationResult applyLocationToSet(
        const dnv::vista::sdk::GmodPath& path, std::size_t setIndex, const dnv::vista::sdk::Location& location);
} // namespace viscore::gmod
