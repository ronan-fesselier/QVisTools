#pragma once

#include <QString>

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>

namespace qvistools::detail
{
    inline std::string toLower(std::string_view s)
    {
        std::string result{ s };
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return result;
    }

    inline QString toQString(std::string_view s)
    {
        return QString::fromUtf8(s.data(), static_cast<int>(s.size()));
    }

    inline int naturalCompare(std::string_view a, std::string_view b)
    {
        std::size_t i{ 0 };
        std::size_t j{ 0 };

        while (i < a.size() && j < b.size())
        {
            if (std::isdigit(static_cast<unsigned char>(a[i])) && std::isdigit(static_cast<unsigned char>(b[j])))
            {
                std::size_t ni{ i };
                std::size_t nj{ j };

                while (ni < a.size() && std::isdigit(static_cast<unsigned char>(a[ni])))
                {
                    ++ni;
                }
                while (nj < b.size() && std::isdigit(static_cast<unsigned char>(b[nj])))
                {
                    ++nj;
                }

                const auto runA{ a.substr(i, ni - i) };
                const auto runB{ b.substr(j, nj - j) };

                const auto significantA{ runA.find_first_not_of('0') };
                const auto significantB{ runB.find_first_not_of('0') };
                const auto digitsA{ significantA == std::string_view::npos ? std::string_view{}
                                                                           : runA.substr(significantA) };
                const auto digitsB{ significantB == std::string_view::npos ? std::string_view{}
                                                                           : runB.substr(significantB) };

                if (digitsA.size() != digitsB.size())
                {
                    return digitsA.size() < digitsB.size() ? -1 : 1;
                }
                if (const int cmp{ digitsA.compare(digitsB) }; cmp != 0)
                {
                    return cmp;
                }

                i = ni;
                j = nj;
            }
            else
            {
                if (a[i] != b[j])
                {
                    return a[i] < b[j] ? -1 : 1;
                }
                ++i;
                ++j;
            }
        }
        if (a.size() == b.size())
        {
            return 0;
        }
        return a.size() < b.size() ? -1 : 1;
    }
} // namespace qvistools::detail
