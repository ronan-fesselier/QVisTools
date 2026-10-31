#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace dnv::vista::sdk
{
    class Gmod;
    class GmodNode;
} // namespace dnv::vista::sdk

namespace qvistools::core::vis
{
    /**
     * @brief Case-insensitive substring search index over a Gmod's nodes
     *        (code, name, common name, definition)
     * @details Backed by a trigram index: each entry's text fields are split into
     *          overlapping 3-character windows, mapped to the entries containing them.
     *          A search first narrows candidates to entries sharing every trigram of the
     *          search term, then confirms with an exact substring check on those candidates
     *          only - avoiding a full scan of every node for typical (length >= 3) terms.
     *          Terms shorter than 3 characters are not selective enough to index and fall
     *          back to a full scan.
     *
     *          Every node is indexed by its own text fields, but a Match is only reported
     *          for a (node, parent) pair that is actually a displayed tree row: a node
     *          hidden behind its parent's product type/selection substitution is indexed
     *          (so its text is still searchable) but never reported as reachable through
     *          that parent, since no such row exists in the tree
     */
    class SearchIndex final
    {
    public:
        /**
         * @brief A search result
         * @details node/parent/skippedParent point to nodes owned by the Gmod passed to
         *          the SearchIndex constructor. The Gmod must outlive the SearchIndex and
         *          any Match copies obtained from it
         */
        struct Match
        {
            const dnv::vista::sdk::GmodNode* node;
            const dnv::vista::sdk::GmodNode* parent;
            const dnv::vista::sdk::GmodNode* skippedParent;
        };

        explicit SearchIndex(const dnv::vista::sdk::Gmod& gmod);

        [[nodiscard]] std::vector<Match> search(std::string_view term) const;

    private:
        struct Entry
        {
            const dnv::vista::sdk::GmodNode* node;
            std::string_view code;
            std::string_view name;
            std::string_view commonName;
            std::string_view definition;
        };

        [[nodiscard]] bool entryMatches(const Entry& entry, std::string_view termLower) const;
        [[nodiscard]] std::vector<std::size_t> candidateEntryIndices(std::string_view termLower) const;
        [[nodiscard]] std::vector<Match> matchesForEntry(const Entry& entry) const;

        std::vector<Entry> m_entries;
        std::unordered_map<std::string, std::vector<std::size_t>> m_trigramIndex;
    };
} // namespace qvistools::core::vis
