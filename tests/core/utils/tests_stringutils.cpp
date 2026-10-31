#include "core/utils/StringUtils.h"

#include <doctest/doctest.h>

TEST_SUITE("StringUtils")
{
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
