#pragma once

#include <cctype>
#include <compare>
#include <cstddef>
#include <string_view>

namespace viscore::utils
{
    inline bool isAsciiDigit(unsigned char c)
    {
        return c >= '0' && c <= '9';
    }

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
} // namespace viscore::utils
