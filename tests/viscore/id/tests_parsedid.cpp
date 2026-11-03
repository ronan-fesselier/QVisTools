#include <viscore/id/ParseId.h>

#include <doctest/doctest.h>

TEST_SUITE("ParseId")
{
    TEST_CASE("parseExistingId accepts a Universal ID produced by buildUniversalId")
    {
        const auto result = viscore::id::parseExistingId(
            "data.dnv.com/IMO1234567/dnv-v2/vis-3-11a/411.1/C101.63/S206/meta/qty-pressure/cnt-fresh.cooling.water/"
            "pos-inlet/detail-jacket");
        REQUIRE(result.id.has_value());
        CHECK(result.id->imoNumberText.has_value());
    }
}
