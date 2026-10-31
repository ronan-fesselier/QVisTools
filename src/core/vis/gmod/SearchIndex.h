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
     * @brief Case-insensitive search index over a Gmod's nodes (code, name, common
     *        name, definition)
     * @details Two complementary strategies, combined like DNV's own Lucene + Basic
     *          search strategies, each backed by its own inverted index so that
     *          neither strategy can exclude a candidate the other would have found:
     *          - Stemmed-word match: free-text fields (not the code) are split into
     *            words, each stripped of a trailing plural 's'. A match requires EVERY
     *            stemmed word of the term to appear, so "cooling pump" matches "cooling"
     *            + "pump"/"pumps" in any field and order, but not a partial word like "pum"
     *          - Substring match: the term matches as a literal substring of a field, so
     *            "C101" matches "C101.31" even though neither is a whole word
     *          Candidates come from the union of both indexes, confirmed by exact checks -
     *          avoiding a full scan for typical (length >= 3) terms. Shorter terms fall
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
            std::vector<std::string> stemmedWords;
        };

        [[nodiscard]] bool entryMatches(const Entry& entry, std::string_view termLower) const;
        [[nodiscard]] std::vector<std::size_t> candidateEntryIndices(std::string_view termLower) const;
        [[nodiscard]] std::vector<Match> matchesForEntry(const Entry& entry) const;

        std::vector<Entry> m_entries;
        std::unordered_map<std::string, std::vector<std::size_t>> m_trigramIndex;
        std::unordered_map<std::string, std::vector<std::size_t>> m_stemIndex;
    };
} // namespace qvistools::core::vis
