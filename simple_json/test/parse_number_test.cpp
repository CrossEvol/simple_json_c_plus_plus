#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <static/simple_json.h>

#include "catch2/catch_approx.hpp"

// Helper function to test number parsing
void EXPECT_NUMBER(long double expected, const std::string &json) {
    const auto node = Decoder::parse(json);
    REQUIRE(node->getType() == NodeType::Number);
    auto approx = Catch::Approx(expected)
            .epsilon(std::numeric_limits<long double>::epsilon())
            .margin(std::numeric_limits<long double>::min());
    REQUIRE(node->getNumber() == approx);
}

TEST_CASE("Parse numbers", "[parse_number]") {
    SECTION("Basic numbers") {
        EXPECT_NUMBER(0.0, "0");
        EXPECT_NUMBER(0.0, "-0");
        EXPECT_NUMBER(0.0, "-0.0");
        EXPECT_NUMBER(1.0, "1");
        EXPECT_NUMBER(-1.0, "-1");
        EXPECT_NUMBER(1.5, "1.5");
        EXPECT_NUMBER(-1.5, "-1.5");
        EXPECT_NUMBER(3.1416, "3.1416");
    }

    SECTION("Scientific notation") {
        EXPECT_NUMBER(1e10, "1E10");
        EXPECT_NUMBER(1e10, "1e10");
        EXPECT_NUMBER(1e+10, "1E+10");
        EXPECT_NUMBER(1e-10, "1E-10");
        EXPECT_NUMBER(-1e10, "-1E10");
        EXPECT_NUMBER(-1e10, "-1e10");
        EXPECT_NUMBER(-1e+10, "-1E+10");
        EXPECT_NUMBER(-1e-10, "-1E-10");
        EXPECT_NUMBER(1.234e+10, "1.234E+10");
        EXPECT_NUMBER(1.234e-10, "1.234E-10");
        // TODO: here must underflow , but not now
        // EXPECT_NUMBER(0.0, "1e-10000"); // must underflow
    }

    SECTION("Edge cases") {
        EXPECT_NUMBER(1.0000000000000002, "1.0000000000000002");
        EXPECT_NUMBER(4.9406564584124654e-324, "4.9406564584124654e-324");
        EXPECT_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
        EXPECT_NUMBER(2.2250738585072009e-308, "2.2250738585072009e-308");
        EXPECT_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
        EXPECT_NUMBER(2.2250738585072014e-308, "2.2250738585072014e-308");
        EXPECT_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
        EXPECT_NUMBER(1.7976931348623157e+308, "1.7976931348623157e+308");
        EXPECT_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
    }
}
