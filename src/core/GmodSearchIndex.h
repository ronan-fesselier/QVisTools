#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace dnv::vista::sdk
{
    class Gmod;
    class GmodNode;
} // namespace dnv::vista::sdk

namespace qvistools::core
{
    class GmodSearchIndex final
    {
    public:
        struct Match
        {
            const dnv::vista::sdk::GmodNode* node;
            const dnv::vista::sdk::GmodNode* parent;
            const dnv::vista::sdk::GmodNode* skippedParent;
        };

        explicit GmodSearchIndex(const dnv::vista::sdk::Gmod& gmod);

        [[nodiscard]] std::vector<Match> search(std::string_view term) const;

    private:
        struct Entry
        {
            const dnv::vista::sdk::GmodNode* node;
            std::string codeLower;
            std::string nameLower;
            std::string commonNameLower;
            std::string definitionLower;
        };

        std::vector<Entry> m_entries;
    };
} // namespace qvistools::core
