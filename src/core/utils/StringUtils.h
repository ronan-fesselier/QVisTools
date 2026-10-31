#pragma once

#include <algorithm>
#include <cctype>
#include <compare>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace qvistools::core::utils
{
    /**
     * @brief Lowercase an ASCII string
     */
    inline std::string toLower(std::string_view s)
    {
        std::string result{ s };
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return result;
    }

    /**
     * @brief Check whether a byte, treated as ASCII, equals another byte ignoring case
     */
    inline bool asciiEqualCaseInsensitive(unsigned char a, unsigned char b)
    {
        const auto lower{ [](unsigned char c) {
            return (c >= 'A' && c <= 'Z') ? static_cast<unsigned char>(c + ('a' - 'A')) : c;
        } };
        return lower(a) == lower(b);
    }

    /**
     * @brief Check whether haystack contains needle as a substring, ignoring ASCII case
     * @details Avoids allocating a lowercased copy of either argument - needed since
     *          Entry stores string_view into the Gmod's own (mixed-case) text, not a
     *          lowercased copy
     */
    inline bool containsCaseInsensitive(std::string_view haystack, std::string_view needle)
    {
        if (needle.empty())
        {
            return true;
        }
        if (needle.size() > haystack.size())
        {
            return false;
        }

        for (std::size_t i{ 0 }; i + needle.size() <= haystack.size(); ++i)
        {
            bool allMatch{ true };
            for (std::size_t j{ 0 }; j < needle.size(); ++j)
            {
                if (!asciiEqualCaseInsensitive(
                        static_cast<unsigned char>(haystack[i + j]), static_cast<unsigned char>(needle[j])))
                {
                    allMatch = false;
                    break;
                }
            }
            if (allMatch)
            {
                return true;
            }
        }

        return false;
    }

    /**
     * @brief Strip a trailing plural 's' from a lowercase word
     * @details Simple heuristic: strips a single trailing 's' only for words longer
     *          than 3 characters and not already ending in a double 's', so "pumps" ->
     *          "pump" but short words ("is", "as", "gas", "bus") and double-s words
     *          ("class", "process") are left untouched. Does not handle -es/-ies or
     *          irregular plurals
     */
    inline std::string_view stemSimple(std::string_view word)
    {
        if (word.size() > 3 && word.back() == 's' && word[word.size() - 2] != 's')
        {
            return word.substr(0, word.size() - 1);
        }

        return word;
    }

    /**
     * @brief Check whether a byte is an ASCII digit
     * @details Unlike std::isdigit, this is never affected by the active C locale
     */
    inline bool isAsciiDigit(unsigned char c)
    {
        return c >= '0' && c <= '9';
    }

    /**
     * @brief Check whether a byte is an ASCII letter or digit
     * @details Unlike std::isalnum, this is never affected by the active C locale -
     *          Gmod codes/names/definitions are ASCII, so word-splitting must not change
     *          behavior if something elsewhere in the process calls setlocale()
     */
    inline bool isAsciiAlnum(unsigned char c)
    {
        return isAsciiDigit(c) || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }

    /**
     * @brief Split a string into alphanumeric words
     * @details Any non-alphanumeric character (space, punctuation, parentheses, ...)
     *          is treated as a separator
     */
    inline std::vector<std::string> splitWords(std::string_view text)
    {
        std::vector<std::string> words;

        std::size_t i{ 0 };
        while (i < text.size())
        {
            while (i < text.size() && !isAsciiAlnum(static_cast<unsigned char>(text[i])))
            {
                ++i;
            }

            const std::size_t start{ i };
            while (i < text.size() && isAsciiAlnum(static_cast<unsigned char>(text[i])))
            {
                ++i;
            }

            if (i > start)
            {
                words.emplace_back(text.substr(start, i - start));
            }
        }

        return words;
    }

    /**
     * @brief Compare two strings using natural order (numeric runs compared by value)
     * @details Non-numeric characters compare lexicographically. Runs of digits are
     *          compared as numbers (leading zeros ignored), so "a2" < "a10"
     */
    inline std::strong_ordering naturalCompare(std::string_view a, std::string_view b)
    {
        std::size_t i{ 0 };
        std::size_t j{ 0 };

        while (i < a.size() && j < b.size())
        {
            if (isAsciiDigit(static_cast<unsigned char>(a[i])) && isAsciiDigit(static_cast<unsigned char>(b[j])))
            {
                std::size_t ai{ i };
                std::size_t bj{ j };

                while (ai < a.size() && a[ai] == '0')
                {
                    ++ai;
                }
                while (bj < b.size() && b[bj] == '0')
                {
                    ++bj;
                }

                std::size_t ni{ ai };
                std::size_t nj{ bj };

                while (ni < a.size() && isAsciiDigit(static_cast<unsigned char>(a[ni])))
                {
                    ++ni;
                }
                while (nj < b.size() && isAsciiDigit(static_cast<unsigned char>(b[nj])))
                {
                    ++nj;
                }

                const auto digitsA{ a.substr(ai, ni - ai) };
                const auto digitsB{ b.substr(bj, nj - bj) };

                if (digitsA.size() != digitsB.size())
                {
                    return digitsA.size() <=> digitsB.size();
                }
                if (const auto cmp{ digitsA.compare(digitsB) }; cmp != 0)
                {
                    return cmp <=> 0;
                }

                i = ni;
                j = nj;
            }
            else
            {
                if (a[i] != b[j])
                {
                    return static_cast<unsigned char>(a[i]) <=> static_cast<unsigned char>(b[j]);
                }
                ++i;
                ++j;
            }
        }
        return (a.size() - i) <=> (b.size() - j);
    }
} // namespace qvistools::core::utils
