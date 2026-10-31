#include "core/utils/StringUtils.h"

#include <doctest/doctest.h>

TEST_SUITE("StringUtils")
{
    TEST_CASE("asciiEqualCaseInsensitive matches letters regardless of case")
    {
        CHECK(qvistools::core::utils::asciiEqualCaseInsensitive('a', 'A'));
        CHECK(qvistools::core::utils::asciiEqualCaseInsensitive('Z', 'z'));
        CHECK_FALSE(qvistools::core::utils::asciiEqualCaseInsensitive('a', 'b'));
    }

    TEST_CASE("asciiEqualCaseInsensitive leaves non-letter bytes unaffected by case folding")
    {
        CHECK(qvistools::core::utils::asciiEqualCaseInsensitive('5', '5'));
        CHECK_FALSE(qvistools::core::utils::asciiEqualCaseInsensitive('5', '6'));
    }

    TEST_CASE("containsCaseInsensitive matches a substring regardless of case")
    {
        CHECK(qvistools::core::utils::containsCaseInsensitive("Reciprocating Pump", "pump"));
        CHECK(qvistools::core::utils::containsCaseInsensitive("Reciprocating Pump", "RECIPROCATING"));
        CHECK_FALSE(qvistools::core::utils::containsCaseInsensitive("Reciprocating Pump", "valve"));
    }

    TEST_CASE("containsCaseInsensitive treats an empty needle as always matching")
    {
        CHECK(qvistools::core::utils::containsCaseInsensitive("anything", ""));
        CHECK(qvistools::core::utils::containsCaseInsensitive("", ""));
    }

    TEST_CASE("containsCaseInsensitive does not match a needle longer than the haystack")
    {
        CHECK_FALSE(qvistools::core::utils::containsCaseInsensitive("ab", "abc"));
    }

    TEST_CASE("stemSimple strips a single trailing 's'")
    {
        CHECK(qvistools::core::utils::stemSimple("pumps") == "pump");
        CHECK(qvistools::core::utils::stemSimple("valves") == "valve");
    }

    TEST_CASE("stemSimple leaves words without a trailing 's' untouched")
    {
        CHECK(qvistools::core::utils::stemSimple("pump") == "pump");
    }

    TEST_CASE("stemSimple does not strip the only character of a single-letter word")
    {
        CHECK(qvistools::core::utils::stemSimple("s") == "s");
    }

    TEST_CASE("stemSimple leaves short words ending in a single 's' untouched")
    {
        CHECK(qvistools::core::utils::stemSimple("as") == "as");
        CHECK(qvistools::core::utils::stemSimple("gas") == "gas");
        CHECK(qvistools::core::utils::stemSimple("bus") == "bus");
    }

    TEST_CASE("stemSimple leaves words ending in a double 's' untouched")
    {
        CHECK(qvistools::core::utils::stemSimple("class") == "class");
        CHECK(qvistools::core::utils::stemSimple("process") == "process");
    }

    TEST_CASE("splitWords splits on whitespace")
    {
        const auto words{ qvistools::core::utils::splitWords("reciprocating internal combustion engine") };

        REQUIRE(words.size() == 4);
        CHECK(words[0] == "reciprocating");
        CHECK(words[1] == "internal");
        CHECK(words[2] == "combustion");
        CHECK(words[3] == "engine");
    }

    TEST_CASE("splitWords splits on punctuation, including parentheses and hyphens")
    {
        const auto words{ qvistools::core::utils::splitWords("pump(s) - reciprocating, internal.") };

        REQUIRE(words.size() == 4);
        CHECK(words[0] == "pump");
        CHECK(words[1] == "s");
        CHECK(words[2] == "reciprocating");
        CHECK(words[3] == "internal");
    }

    TEST_CASE("splitWords ignores leading, trailing and repeated separators")
    {
        const auto words{ qvistools::core::utils::splitWords("  pump   valve  ") };

        REQUIRE(words.size() == 2);
        CHECK(words[0] == "pump");
        CHECK(words[1] == "valve");
    }

    TEST_CASE("splitWords returns an empty vector for a string with no words")
    {
        CHECK(qvistools::core::utils::splitWords("   -- ,, ").empty());
        CHECK(qvistools::core::utils::splitWords("").empty());
    }

    TEST_CASE("naturalCompare orders numeric runs by value, not lexicographically")
    {
        CHECK(qvistools::core::utils::naturalCompare("a2", "a10") == std::strong_ordering::less);
        CHECK(qvistools::core::utils::naturalCompare("a10", "a2") == std::strong_ordering::greater);
    }

    TEST_CASE("naturalCompare ignores leading zeros in numeric runs")
    {
        CHECK(qvistools::core::utils::naturalCompare("a010", "a10") == std::strong_ordering::equal);
    }

    TEST_CASE("naturalCompare falls back to lexicographic order for non-numeric runs")
    {
        CHECK(qvistools::core::utils::naturalCompare("abc", "abd") == std::strong_ordering::less);
        CHECK(qvistools::core::utils::naturalCompare("abc", "abc") == std::strong_ordering::equal);
    }

    TEST_CASE("naturalCompare orders shorter strings first when one is a prefix of the other")
    {
        CHECK(qvistools::core::utils::naturalCompare("ab", "abc") == std::strong_ordering::less);
    }

    TEST_CASE("naturalCompare compares non-numeric bytes as unsigned, not as signed char")
    {
        // 0x7F (127) is positive in both signed and unsigned char, but 0x80 is 128
        // unsigned vs -128 signed: a signed-char comparison would wrongly order 0x7F
        // after 0x80 (127 > -128), while the correct unsigned order is 0x7F before 0x80
        CHECK(qvistools::core::utils::naturalCompare("\x7F", "\x80") == std::strong_ordering::less);
    }
}
