#pragma once

#include <dnv/vista/sdk/core/CodebookName.h>

#include <array>
#include <string>
#include <vector>

namespace dnv::vista::sdk
{
    class Codebooks;
} // namespace dnv::vista::sdk

namespace qvistools::core::vis
{
    /**
     * @brief The 8 codebooks shown to the user, in display order (Quantity, Content,
     *        Calculation, State, Command, Type, Position, Detail)
     */
    constexpr std::array<dnv::vista::sdk::CodebookName, 8> DISPLAYED_CODEBOOKS{
        dnv::vista::sdk::CodebookName::Quantity,    dnv::vista::sdk::CodebookName::Content,
        dnv::vista::sdk::CodebookName::Calculation, dnv::vista::sdk::CodebookName::State,
        dnv::vista::sdk::CodebookName::Command,     dnv::vista::sdk::CodebookName::Type,
        dnv::vista::sdk::CodebookName::Position,    dnv::vista::sdk::CodebookName::Detail
    };

    /**
     * @brief Standard (non-custom) values for a codebook, sorted for stable display
     * @details Backs a metadata field's autocomplete - createTag() still accepts values
     *          outside this list (custom values), this is only the suggestion set
     */
    [[nodiscard]] std::vector<std::string> standardValuesOf(
        const dnv::vista::sdk::Codebooks& codebooks, dnv::vista::sdk::CodebookName name);
} // namespace qvistools::core::vis
