#pragma once

#include <dnv/vista/sdk/core/CodebookName.h>

#include <array>
#include <string>
#include <vector>

namespace dnv::vista::sdk
{
    class Codebooks;
} // namespace dnv::vista::sdk

namespace viscore::codebooks
{
    constexpr std::array<dnv::vista::sdk::CodebookName, 8> DISPLAYED_CODEBOOKS{
        dnv::vista::sdk::CodebookName::Quantity,    dnv::vista::sdk::CodebookName::Content,
        dnv::vista::sdk::CodebookName::Calculation, dnv::vista::sdk::CodebookName::State,
        dnv::vista::sdk::CodebookName::Command,     dnv::vista::sdk::CodebookName::Type,
        dnv::vista::sdk::CodebookName::Position,    dnv::vista::sdk::CodebookName::Detail
    };

    [[nodiscard]] std::vector<std::string> standardValuesOf(
        const dnv::vista::sdk::Codebooks& codebooks, dnv::vista::sdk::CodebookName name);
} // namespace viscore::codebooks
