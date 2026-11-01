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

namespace viscore::gmod
{
    class SearchIndex final
    {
    public:
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
            std::string code;
            std::string name;
            std::string commonName;
            std::string definition;
        };

        [[nodiscard]] bool entryMatches(const Entry& entry, std::string_view termLower) const;
        [[nodiscard]] std::vector<std::size_t> candidateEntryIndices(std::string_view termLower) const;
        [[nodiscard]] std::vector<Match> matchesForEntry(const Entry& entry) const;

        std::vector<Entry> m_entries;
        std::unordered_map<std::string, std::vector<std::size_t>> m_trigramIndex;
    };
} // namespace viscore::gmod
