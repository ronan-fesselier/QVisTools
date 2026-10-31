#include <viscore/utils/StringUtils.h>

#include <doctest/doctest.h>

TEST_SUITE("StringUtils")
{
    TEST_CASE("naturalCompare orders numeric runs by value, not lexicographically")
    {
        CHECK(viscore::utils::naturalCompare("a2", "a10") == std::strong_ordering::less);
        CHECK(viscore::utils::naturalCompare("a10", "a2") == std::strong_ordering::greater);
    }

    TEST_CASE("naturalCompare ignores leading zeros in numeric runs")
    {
        CHECK(viscore::utils::naturalCompare("a010", "a10") == std::strong_ordering::equal);
    }

    TEST_CASE("naturalCompare falls back to lexicographic order for non-numeric runs")
    {
        CHECK(viscore::utils::naturalCompare("abc", "abd") == std::strong_ordering::less);
        CHECK(viscore::utils::naturalCompare("abc", "abc") == std::strong_ordering::equal);
    }

    TEST_CASE("naturalCompare orders shorter strings first when one is a prefix of the other")
    {
        CHECK(viscore::utils::naturalCompare("ab", "abc") == std::strong_ordering::less);
    }

    TEST_CASE("naturalCompare compares non-numeric bytes as unsigned, not as signed char")
    {
        CHECK(viscore::utils::naturalCompare("\x7F", "\x80") == std::strong_ordering::less);
    }
}
