#include "SearchIndex.h"

#include "core/utils/StringUtils.h"
#include "core/vis/gmod/GmodNodeChildren.h"

#include <dnv/vista/sdk/core/Gmod.h>
#include <dnv/vista/sdk/core/GmodNode.h>

#include <algorithm>
#include <iterator>

namespace qvistools::core::vis
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

        std::vector<std::string> stemmedWordsOf(std::string_view field)
        {
            std::vector<std::string> words{ utils::splitWords(field) };
            for (auto& word : words)
            {
                std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) {
                    return static_cast<char>((c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c);
                });
                word.resize(utils::stemSimple(word).size());
            }
            return words;
        }

        void mergeIndices(std::vector<std::size_t>& target, const std::vector<std::size_t>& toAdd)
        {
            std::vector<std::size_t> merged;
            merged.reserve(target.size() + toAdd.size());
            std::set_union(target.begin(), target.end(), toAdd.begin(), toAdd.end(), std::back_inserter(merged));
            target = std::move(merged);
        }
    } // namespace

    SearchIndex::SearchIndex(const dnv::vista::sdk::Gmod& gmod)
    {
        m_entries.reserve(static_cast<std::size_t>(std::distance(gmod.begin(), gmod.end())));

        for (const auto& [code, node] : gmod)
        {
            const auto& metadata{ node.metadata() };

            const std::string_view nameView{ metadata.name() };
            const std::string_view commonNameView{ metadata.commonName().has_value()
                                                       ? std::string_view{ *metadata.commonName() }
                                                       : std::string_view{} };
            const std::string_view definitionView{ metadata.definition().has_value()
                                                       ? std::string_view{ *metadata.definition() }
                                                       : std::string_view{} };

            // Codes (e.g. "411.1", "C101") are identifiers, not natural-language words,
            // so stemming only applies to the free-text fields (name/commonName/definition)
            std::vector<std::string> stemmedWords{ stemmedWordsOf(nameView) };
            for (auto&& word : stemmedWordsOf(commonNameView))
            {
                stemmedWords.push_back(std::move(word));
            }
            for (auto&& word : stemmedWordsOf(definitionView))
            {
                stemmedWords.push_back(std::move(word));
            }

            std::sort(stemmedWords.begin(), stemmedWords.end());
            stemmedWords.erase(std::unique(stemmedWords.begin(), stemmedWords.end()), stemmedWords.end());

            const auto entryIndex{ m_entries.size() };

            for (const auto& word : stemmedWords)
            {
                auto& indices{ m_stemIndex[word] };
                if (indices.empty() || indices.back() != entryIndex)
                {
                    indices.push_back(entryIndex);
                }
            }

            m_entries.push_back(Entry{ &node,
                                       std::string_view{ node.code() },
                                       nameView,
                                       commonNameView,
                                       definitionView,
                                       std::move(stemmedWords) });

            const auto& entry{ m_entries.back() };
            indexField(m_trigramIndex, entry.code, entryIndex);
            indexField(m_trigramIndex, entry.name, entryIndex);
            indexField(m_trigramIndex, entry.commonName, entryIndex);
            indexField(m_trigramIndex, entry.definition, entryIndex);
        }
    }

    bool SearchIndex::entryMatches(const Entry& entry, std::string_view termLower) const
    {
        const bool substringMatch{ utils::containsCaseInsensitive(entry.code, termLower) ||
                                   utils::containsCaseInsensitive(entry.name, termLower) ||
                                   utils::containsCaseInsensitive(entry.commonName, termLower) ||
                                   utils::containsCaseInsensitive(entry.definition, termLower) };
        if (substringMatch)
        {
            return true;
        }

        const auto termWords{ utils::splitWords(termLower) };
        if (termWords.empty())
        {
            return false;
        }

        return std::all_of(termWords.begin(), termWords.end(), [&entry](const auto& word) {
            const auto stemmedWord{ utils::stemSimple(word) };
            return std::binary_search(entry.stemmedWords.begin(), entry.stemmedWords.end(), stemmedWord);
        });
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

        std::vector<std::size_t> trigramCandidates;
        bool first{ true };
        bool trigramMiss{ false };

        for (std::size_t i{ 0 }; i + TRIGRAM_LENGTH <= termLower.size() && !trigramMiss; ++i)
        {
            const auto trigram{ std::string{ termLower.substr(i, TRIGRAM_LENGTH) } };

            const auto it{ m_trigramIndex.find(trigram) };
            if (it == m_trigramIndex.end())
            {
                trigramMiss = true;
                break;
            }

            if (first)
            {
                trigramCandidates = it->second;
                first = false;
                continue;
            }

            // Both trigramCandidates and it->second are sorted ascending (entries are
            // indexed in increasing entryIndex order), so a linear sorted-set intersection
            // avoids the per-trigram hash set allocation
            std::vector<std::size_t> intersected;
            intersected.reserve(trigramCandidates.size());
            std::set_intersection(
                trigramCandidates.begin(),
                trigramCandidates.end(),
                it->second.begin(),
                it->second.end(),
                std::back_inserter(intersected));
            trigramCandidates = std::move(intersected);

            if (trigramCandidates.empty())
            {
                break;
            }
        }

        if (trigramMiss)
        {
            // No entry contains every trigram of the raw term, so the substring strategy
            // has nothing to offer - but the stem strategy below is independent and may
            // still find entries via a whole-word stemmed match
            trigramCandidates.clear();
        }

        // Candidates from the stem strategy: any entry containing at least one of the
        // term's stemmed words is a candidate - entryMatches() then requires ALL of
        // them to be present before confirming a match
        for (const auto& word : utils::splitWords(termLower))
        {
            const auto stemIt{ m_stemIndex.find(std::string{ utils::stemSimple(word) }) };
            if (stemIt != m_stemIndex.end())
            {
                mergeIndices(trigramCandidates, stemIt->second);
            }
        }

        return trigramCandidates;
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
} // namespace qvistools::core::vis
