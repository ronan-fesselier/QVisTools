#include <viscore/utils/StringUtils.h>

#include <doctest/doctest.h>

TEST_SUITE("StringUtils")
{
    TEST_CASE("asciiEqualCaseInsensitive matches letters regardless of case")
    {
        CHECK(viscore::utils::asciiEqualCaseInsensitive('a', 'A'));
        CHECK(viscore::utils::asciiEqualCaseInsensitive('Z', 'z'));
        CHECK_FALSE(viscore::utils::asciiEqualCaseInsensitive('a', 'b'));
    }

    TEST_CASE("asciiEqualCaseInsensitive leaves non-letter bytes unaffected by case folding")
    {
        CHECK(viscore::utils::asciiEqualCaseInsensitive('5', '5'));
        CHECK_FALSE(viscore::utils::asciiEqualCaseInsensitive('5', '6'));
    }

    TEST_CASE("containsCaseInsensitive matches a substring regardless of case")
    {
        CHECK(viscore::utils::containsCaseInsensitive("Reciprocating Pump", "pump"));
        CHECK(viscore::utils::containsCaseInsensitive("Reciprocating Pump", "RECIPROCATING"));
        CHECK_FALSE(viscore::utils::containsCaseInsensitive("Reciprocating Pump", "valve"));
    }

    TEST_CASE("containsCaseInsensitive treats an empty needle as always matching")
    {
        CHECK(viscore::utils::containsCaseInsensitive("anything", ""));
        CHECK(viscore::utils::containsCaseInsensitive("", ""));
    }

    TEST_CASE("containsCaseInsensitive does not match a needle longer than the haystack")
    {
        CHECK_FALSE(viscore::utils::containsCaseInsensitive("ab", "abc"));
    }

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
