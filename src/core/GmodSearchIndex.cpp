#include "GmodSearchIndex.h"

#include "detail/StringUtils.h"

#include <dnv/vista/sdk/core/Gmod.h>
#include <dnv/vista/sdk/core/GmodNode.h>

namespace qvistools::core
{
    GmodSearchIndex::GmodSearchIndex(const dnv::vista::sdk::Gmod& gmod)
    {
        m_entries.reserve(8000);

        for (const auto& [code, node] : gmod)
        {
            const auto& metadata{ node.metadata() };

            m_entries.push_back(
                Entry{ &node,
                       detail::toLower(node.code()),
                       detail::toLower(metadata.name()),
                       metadata.commonName().has_value() ? detail::toLower(*metadata.commonName()) : std::string{},
                       metadata.definition().has_value() ? detail::toLower(*metadata.definition()) : std::string{} });
        }
    }

    std::vector<GmodSearchIndex::Match> GmodSearchIndex::search(std::string_view term) const
    {
        const auto termLower{ detail::toLower(term) };

        std::vector<Match> matches;
        for (const auto& entry : m_entries)
        {
            const bool matched{ entry.codeLower.find(termLower) != std::string::npos ||
                                entry.nameLower.find(termLower) != std::string::npos ||
                                entry.commonNameLower.find(termLower) != std::string::npos ||
                                entry.definitionLower.find(termLower) != std::string::npos };

            if (matched)
            {
                if (entry.node->parents().isEmpty())
                {
                    matches.push_back(Match{ entry.node, nullptr, nullptr });
                }
                else
                {
                    for (const auto* parent : entry.node->parents())
                    {
                        bool skippedAny{ false };
                        for (const auto* grandParent : parent->parents())
                        {
                            if (dnv::vista::sdk::Gmod::isProductSelectionAssignment(grandParent, parent))
                            {
                                matches.push_back(Match{ entry.node, grandParent, parent });
                                skippedAny = true;
                            }
                        }

                        if (!skippedAny)
                        {
                            matches.push_back(Match{ entry.node, parent, nullptr });
                        }
                    }
                }
            }
        }

        return matches;
    }
} // namespace qvistools::core
