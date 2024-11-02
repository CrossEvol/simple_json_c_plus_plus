#include <catch2/catch_test_macros.hpp>
#include <static/simple_json.h>

// Helper macro for testing parse exceptions
#define TEST_PARSE_EXCEPTION(exception_type, json) \
    REQUIRE_THROWS_AS(Decoder::parse(json), exception_type)

TEST_CASE("Parse expect value exceptions", "[parse_exception]") {
    TEST_PARSE_EXCEPTION(ParseExpectValueException, "");
    TEST_PARSE_EXCEPTION(ParseExpectValueException, " ");
}

TEST_CASE("Parse invalid value exceptions", "[parse_exception]") {
    SECTION("Invalid literals") {
        TEST_PARSE_EXCEPTION(ParseInvalidValueException, "nul");
        TEST_PARSE_EXCEPTION(ParseInvalidValueException, "?");
    }

    SECTION("Invalid numbers") {
        TEST_PARSE_EXCEPTION(ParseInvalidValueException, "+0");
        TEST_PARSE_EXCEPTION(ParseInvalidValueException, "+1");
        TEST_PARSE_EXCEPTION(ParseInvalidValueException, ".123");
        TEST_PARSE_EXCEPTION(ParseInvalidValueException, "1.");
        TEST_PARSE_EXCEPTION(ParseInvalidValueException, "INF");
        TEST_PARSE_EXCEPTION(ParseInvalidValueException, "inf");
        TEST_PARSE_EXCEPTION(ParseInvalidValueException, "NAN");
        TEST_PARSE_EXCEPTION(ParseInvalidValueException, "nan");
    }

    SECTION("Invalid array values") {
        TEST_PARSE_EXCEPTION(ParseInvalidValueException, "[1,]");
        TEST_PARSE_EXCEPTION(ParseInvalidValueException, "[\"a\", nul]");
    }
}

TEST_CASE("Parse root not singular exceptions", "[parse_exception]") {
    TEST_PARSE_EXCEPTION(ParseRootNotSingularException, "null x");

    SECTION("Invalid numbers") {
        TEST_PARSE_EXCEPTION(ParseRootNotSingularException, "0123");
        TEST_PARSE_EXCEPTION(ParseRootNotSingularException, "0x0");
        TEST_PARSE_EXCEPTION(ParseRootNotSingularException, "0x123");
    }
}

TEST_CASE("Parse number too big exceptions", "[parse_exception]") {
    TEST_PARSE_EXCEPTION(ParseNumberTooBigException, "1e309");
    TEST_PARSE_EXCEPTION(ParseNumberTooBigException, "-1e309");
}

TEST_CASE("Parse string exceptions", "[parse_exception]") {
    SECTION("Missing quotation mark") {
        TEST_PARSE_EXCEPTION(ParseMissQuotationMarkException, "\"");
        TEST_PARSE_EXCEPTION(ParseMissQuotationMarkException, "\"abc");
    }

    SECTION("Invalid escape sequences") {
        TEST_PARSE_EXCEPTION(ParseInvalidStringEscapeException, "\"\\v\"");
        TEST_PARSE_EXCEPTION(ParseInvalidStringEscapeException, "\"\\'\"");
        TEST_PARSE_EXCEPTION(ParseInvalidStringEscapeException, "\"\\0\"");
        TEST_PARSE_EXCEPTION(ParseInvalidStringEscapeException, "\"\\x12\"");
    }

    SECTION("Invalid characters") {
        TEST_PARSE_EXCEPTION(ParseInvalidStringCharException, "\"\x01\"");
        TEST_PARSE_EXCEPTION(ParseInvalidStringCharException, "\"\x1F\"");
    }
}

TEST_CASE("Parse unicode exceptions", "[parse_exception]") {
    SECTION("Invalid unicode hex") {
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeHexException, "\"\\u\"");
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeHexException, "\"\\u0\"");
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeHexException, "\"\\u01\"");
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeHexException, "\"\\u012\"");
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeHexException, "\"\\u/000\"");
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeHexException, "\"\\uG000\"");
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeHexException, "\"\\u0/00\"");
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeHexException, "\"\\u0G00\"");
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeHexException, "\"\\u00G0\"");
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeHexException, "\"\\u000/\"");
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeHexException, "\"\\u000G\"");
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeHexException, "\"\\u 123\"");
    }

    SECTION("Invalid unicode surrogate") {
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeSurrogateException, "\"\\uD800\"");
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeSurrogateException, "\"\\uDBFF\"");
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeSurrogateException, "\"\\uD800\\\\\"");
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeSurrogateException, "\"\\uD800\\uDBFF\"");
        TEST_PARSE_EXCEPTION(ParseInvalidUnicodeSurrogateException, "\"\\uD800\\uE000\"");
    }
}

TEST_CASE("Parse array exceptions", "[parse_exception]") {
    TEST_PARSE_EXCEPTION(ParseMissCommaOrSquareBracketException, "[1");
    TEST_PARSE_EXCEPTION(ParseMissCommaOrSquareBracketException, "[1}");
    TEST_PARSE_EXCEPTION(ParseMissCommaOrSquareBracketException, "[1 2");
    TEST_PARSE_EXCEPTION(ParseMissCommaOrSquareBracketException, "[[]");
}

TEST_CASE("Parse object exceptions", "[parse_exception]") {
    SECTION("Missing key") {
        TEST_PARSE_EXCEPTION(ParseMissKeyException, "{:1,");
        TEST_PARSE_EXCEPTION(ParseMissKeyException, "{1:1,");
        TEST_PARSE_EXCEPTION(ParseMissKeyException, "{true:1,");
        TEST_PARSE_EXCEPTION(ParseMissKeyException, "{false:1,");
        TEST_PARSE_EXCEPTION(ParseMissKeyException, "{null:1,");
        TEST_PARSE_EXCEPTION(ParseMissKeyException, "{[]:1,");
        TEST_PARSE_EXCEPTION(ParseMissKeyException, "{{}:1,");
        TEST_PARSE_EXCEPTION(ParseMissKeyException, "{\"a\":1,");
    }

    SECTION("Missing colon") {
        TEST_PARSE_EXCEPTION(ParseMissColonException, "{\"a\"}");
        TEST_PARSE_EXCEPTION(ParseMissColonException, "{\"a\",\"b\"}");
    }

    SECTION("Missing comma or curly bracket") {
        TEST_PARSE_EXCEPTION(ParseMissCommaOrCurlyBracketException, "{\"a\":1");
        TEST_PARSE_EXCEPTION(ParseMissCommaOrCurlyBracketException, "{\"a\":1]");
        TEST_PARSE_EXCEPTION(ParseMissCommaOrCurlyBracketException, "{\"a\":1 \"b\"");
        TEST_PARSE_EXCEPTION(ParseMissCommaOrCurlyBracketException, "{\"a\":{}");
    }
}

