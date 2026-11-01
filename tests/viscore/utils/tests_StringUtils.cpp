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

    TEST_CASE("stemSimple strips a single trailing 's'")
    {
        CHECK(viscore::utils::stemSimple("pumps") == "pump");
        CHECK(viscore::utils::stemSimple("valves") == "valve");
    }

    TEST_CASE("stemSimple leaves words without a trailing 's' untouched")
    {
        CHECK(viscore::utils::stemSimple("pump") == "pump");
    }

    TEST_CASE("stemSimple does not strip the only character of a single-letter word")
    {
        CHECK(viscore::utils::stemSimple("s") == "s");
    }

    TEST_CASE("stemSimple leaves short words ending in a single 's' untouched")
    {
        CHECK(viscore::utils::stemSimple("as") == "as");
        CHECK(viscore::utils::stemSimple("gas") == "gas");
        CHECK(viscore::utils::stemSimple("bus") == "bus");
    }

    TEST_CASE("stemSimple leaves words ending in a double 's' untouched")
    {
        CHECK(viscore::utils::stemSimple("class") == "class");
        CHECK(viscore::utils::stemSimple("process") == "process");
    }

    TEST_CASE("splitWords splits on whitespace")
    {
        const auto words{ viscore::utils::splitWords("reciprocating internal combustion engine") };

        REQUIRE(words.size() == 4);
        CHECK(words[0] == "reciprocating");
        CHECK(words[1] == "internal");
        CHECK(words[2] == "combustion");
        CHECK(words[3] == "engine");
    }

    TEST_CASE("splitWords splits on punctuation, including parentheses and hyphens")
    {
        const auto words{ viscore::utils::splitWords("pump(s) - reciprocating, internal.") };

        REQUIRE(words.size() == 4);
        CHECK(words[0] == "pump");
        CHECK(words[1] == "s");
        CHECK(words[2] == "reciprocating");
        CHECK(words[3] == "internal");
    }

    TEST_CASE("splitWords ignores leading, trailing and repeated separators")
    {
        const auto words{ viscore::utils::splitWords("  pump   valve  ") };

        REQUIRE(words.size() == 2);
        CHECK(words[0] == "pump");
        CHECK(words[1] == "valve");
    }

    TEST_CASE("splitWords returns an empty vector for a string with no words")
    {
        CHECK(viscore::utils::splitWords("   -- ,, ").empty());
        CHECK(viscore::utils::splitWords("").empty());
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
