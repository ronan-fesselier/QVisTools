#include "CodebookValues.h"

#include <dnv/vista/sdk/core/Codebooks.h>

#include <algorithm>

namespace qvistools::core::vis
{
    std::vector<std::string> standardValuesOf(
        const dnv::vista::sdk::Codebooks& codebooks, dnv::vista::sdk::CodebookName name)
    {
        const auto& standardValues{ codebooks[name].standardValues() };

        std::vector<std::string> values(standardValues.begin(), standardValues.end());
        std::sort(values.begin(), values.end());

        return values;
    }
} // namespace qvistools::core::vis
