#include <catch2/catch_test_macros.hpp>
#include <simple_json/simple_json.h>

TEST_CASE("should parse null", "[null]") {
    const auto node = Decoder::parse("null");
    REQUIRE((node->getType() == NodeType::Null));
}

TEST_CASE("should parse true", "[true]") {
    const auto node = Decoder::parse("true");
    REQUIRE((node->getType() == NodeType::True));
    REQUIRE((node->getBool() == true));
}

TEST_CASE("should parse false", "[false]") {
    const auto node = Decoder::parse("false");
    REQUIRE((node->getType() == NodeType::False));
    REQUIRE((node->getBool() == false));
}
