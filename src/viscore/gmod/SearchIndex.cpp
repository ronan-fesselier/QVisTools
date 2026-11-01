#include "SearchIndex.h"

#include "viscore/utils/StringUtils.h"
#include "viscore/gmod/GmodNodeResolution.h"

#include <dnv/vista/sdk/core/Gmod.h>
#include <dnv/vista/sdk/core/GmodNode.h>

#include <algorithm>
#include <unordered_set>

namespace viscore::gmod
{
    namespace
    {
        constexpr std::size_t TRIGRAM_LENGTH{ 3 };

        void indexField(
            std::unordered_map<std::string, std::vector<std::size_t>>& trigramIndex,
            std::string_view field,
            std::size_t entryIndex)
        {
            if (field.size() < TRIGRAM_LENGTH)
            {
                return;
            }

            for (std::size_t i{ 0 }; i + TRIGRAM_LENGTH <= field.size(); ++i)
            {
                auto& indices{ trigramIndex[utils::toLower(field.substr(i, TRIGRAM_LENGTH))] };
                if (indices.empty() || indices.back() != entryIndex)
                {
                    indices.push_back(entryIndex);
                }
            }
        }
    } // namespace

    SearchIndex::SearchIndex(const dnv::vista::sdk::Gmod& gmod)
    {
        m_entries.reserve(static_cast<std::size_t>(std::distance(gmod.begin(), gmod.end())));

        for (const auto& [code, node] : gmod)
        {
            const auto& metadata{ node.metadata() };

            m_entries.push_back(
                Entry{ &node,
                       utils::toLower(node.code()),
                       utils::toLower(metadata.name()),
                       metadata.commonName().has_value() ? utils::toLower(*metadata.commonName()) : std::string{},
                       metadata.definition().has_value() ? utils::toLower(*metadata.definition()) : std::string{} });

            const auto entryIndex{ m_entries.size() - 1 };
            const auto& entry{ m_entries.back() };
            indexField(m_trigramIndex, entry.code, entryIndex);
            indexField(m_trigramIndex, entry.name, entryIndex);
            indexField(m_trigramIndex, entry.commonName, entryIndex);
            indexField(m_trigramIndex, entry.definition, entryIndex);
        }
    }

    bool SearchIndex::entryMatches(const Entry& entry, std::string_view termLower) const
    {
        return utils::containsCaseInsensitive(entry.code, termLower) ||
               utils::containsCaseInsensitive(entry.name, termLower) ||
               utils::containsCaseInsensitive(entry.commonName, termLower) ||
               utils::containsCaseInsensitive(entry.definition, termLower);
    }

    std::vector<std::size_t> SearchIndex::candidateEntryIndices(std::string_view termLower) const
    {
        if (termLower.size() < TRIGRAM_LENGTH)
        {
            std::vector<std::size_t> all(m_entries.size());
            for (std::size_t i{ 0 }; i < all.size(); ++i)
            {
                all[i] = i;
            }
            return all;
        }

        std::vector<std::size_t> candidates;
        bool first{ true };

        for (std::size_t i{ 0 }; i + TRIGRAM_LENGTH <= termLower.size(); ++i)
        {
            const auto trigram{ std::string{ termLower.substr(i, TRIGRAM_LENGTH) } };

            const auto it{ m_trigramIndex.find(trigram) };
            if (it == m_trigramIndex.end())
            {
                return {};
            }

            if (first)
            {
                candidates = it->second;
                first = false;
                continue;
            }

            const std::unordered_set<std::size_t> nextSet{ it->second.begin(), it->second.end() };

            std::vector<std::size_t> intersected;
            intersected.reserve(candidates.size());
            for (const auto index : candidates)
            {
                if (nextSet.contains(index))
                {
                    intersected.push_back(index);
                }
            }
            candidates = std::move(intersected);

            if (candidates.empty())
            {
                return {};
            }
        }

        return candidates;
    }

    std::vector<SearchIndex::Match> SearchIndex::matchesForEntry(const Entry& entry) const
    {
        std::vector<Match> matches;

        if (entry.node->parents().isEmpty())
        {
            matches.push_back(Match{ entry.node, nullptr, nullptr });
            return matches;
        }

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

            if (skippedAny)
            {
                continue;
            }

            const auto* resolvedParent{ effectiveNode(*parent) };
            if (resolvedParent == parent || resolvedParent == entry.node)
            {
                matches.push_back(Match{ entry.node, parent, nullptr });
            }
        }

        return matches;
    }

    std::vector<SearchIndex::Match> SearchIndex::search(std::string_view term) const
    {
        if (term.empty())
        {
            return {};
        }

        const auto termLower{ utils::toLower(term) };

        std::vector<Match> matches;
        for (const auto entryIndex : candidateEntryIndices(termLower))
        {
            const auto& entry{ m_entries[entryIndex] };
            if (!entryMatches(entry, termLower))
            {
                continue;
            }

            auto entryMatchesVec{ matchesForEntry(entry) };
            matches.insert(matches.end(), entryMatchesVec.begin(), entryMatchesVec.end());
        }

        return matches;
    }
} // namespace viscore::gmod
