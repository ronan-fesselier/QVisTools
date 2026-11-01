#pragma once

#include <dnv/vista/sdk/core/GmodPath.h>

#include <cstddef>
#include <optional>
#include <string>

namespace dnv::vista::sdk
{
    class Location;
} // namespace dnv::vista::sdk

namespace qvistools::core::vis
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

    /**
     * @brief Apply a Location to one of a GmodPath's individualizable sets, returning
     *        the resulting path
     * @details setIndex refers to GmodPath::individualizableSets()'s result, not a raw
     *          node index - the set's own nodeIndices() cover every node that shares
     *          the assigned location (e.g. 411/411i/411.1 together)
     */
    [[nodiscard]] ApplyLocationResult applyLocationToSet(
        const dnv::vista::sdk::GmodPath& path, std::size_t setIndex, const dnv::vista::sdk::Location& location);
} // namespace qvistools::core::vis
