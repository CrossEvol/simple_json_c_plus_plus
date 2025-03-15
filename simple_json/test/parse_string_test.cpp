#include <catch2/catch_test_macros.hpp>
#include <simple_json/simple_json.h>

// Helper function to test string parsing
void TEST_STRING(const std::string &expected, const std::string &json) {
    const auto node = Decoder::parse(json);
    REQUIRE(node->getType() == NodeType::String);
    REQUIRE(node->getString() == expected);
}

TEST_CASE("Parse string tests", "[string]") {
    SECTION("Empty string") {
        TEST_STRING("", "\"\"");
    }

    SECTION("Simple string") {
        TEST_STRING("Hello", "\"Hello\"");
    }

    SECTION("String with escape sequences") {
        TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
        TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    }

    SECTION("String with null character") {
        TEST_STRING(std::string("Hello\0World", 11), "\"Hello\\u0000World\"");
    }

    SECTION("Unicode characters") {
        TEST_STRING("\x24", "\"\\u0024\"");         // Dollar sign U+0024
        TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     // Cents sign U+00A2
        TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); // Euro sign U+20AC
    }

    SECTION("Surrogate pairs") {
        TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  // G clef sign U+1D11E
        TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  // G clef sign U+1D11E (lowercase)
    }
}
