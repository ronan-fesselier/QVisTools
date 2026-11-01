#include "GmodPathLocation.h"

#include <dnv/vista/sdk/core/VIS.h>

#include <exception>

namespace viscore::gmod
{
    ApplyLocationResult applyLocationToSet(
        const dnv::vista::sdk::GmodPath& path, std::size_t setIndex, const dnv::vista::sdk::Location& location)
    {
        auto sets{ path.individualizableSets() };
        if (setIndex >= sets.size())
        {
            return { std::nullopt,
                     ApplyLocationError{ ApplyLocationErrorType::OutOfRange,
                                         "setIndex is out of range for this path's individualizable sets" } };
        }

        try
        {
            sets[setIndex].setLocation(location);
            return { sets[setIndex].build(), std::nullopt };
        }
        catch (const std::exception& e)
        {
            return { std::nullopt, ApplyLocationError{ ApplyLocationErrorType::ValidationError, e.what() } };
        }
    }
} // namespace viscore::gmod
