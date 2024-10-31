//
// Created by mizuk on 2024/10/31.
//

#ifndef CONSTANT_H
#define CONSTANT_H
#include <stdexcept>

enum class NodeType {
    Null,
    False,
    True,
    Number,
    String,
    Array,
    Object,
};

enum class NumberState {
    BEGIN,
    ZERO,
    DIGIT,
    DIGIT_1_TO_9,
    DOT,
    EXPONENT,
    SYMBOL,
    END,
};

enum class ParseException {
    PARSE_EXPECT_VALUE,
    PARSE_INVALID_VALUE,
    PARSE_ROOT_NOT_SINGULAR,
    PARSE_NUMBER_TOO_BIG,
    PARSE_MISS_QUOTATION_MARK,
    PARSE_INVALID_STRING_ESCAPE,
    PARSE_INVALID_STRING_CHAR,
    PARSE_INVALID_UNICODE_HEX,
    PARSE_INVALID_UNICODE_SURROGATE,
    PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
    PARSE_MISS_KEY,
    PARSE_MISS_COLON,
    PARSE_MISS_COMMA_OR_CURLY_BRACKET
};


enum class Literal {
    Null,
    True,
    False,
};

// Overload operator+ to convert Literal to string
inline std::string operator+(const Literal &literal) {
    switch (literal) {
        case Literal::Null: return "null";
        case Literal::True: return "true";
        case Literal::False: return "false";
        default: throw std::runtime_error("Unknown literal"); // anyway, this will not throw
    }
}

#endif //CONSTANT_H
