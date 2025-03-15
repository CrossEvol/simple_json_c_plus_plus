//
// Created by mizuk on 2024/10/31.
//

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <string>

class ParseExpectValueException final : public std::runtime_error {
public:
    ParseExpectValueException() : std::runtime_error("Expect value") {
    }
};

class ParseInvalidValueException final : public std::runtime_error {
public:
    ParseInvalidValueException() : std::runtime_error("Invalid value") {
    }
};

class ParseRootNotSingularException final : public std::runtime_error {
public:
    ParseRootNotSingularException() : std::runtime_error("Root not singular") {
    }
};

class ParseNumberTooBigException final : public std::runtime_error {
public:
    ParseNumberTooBigException() : std::runtime_error("Number too big") {
    }
};

class ParseMissQuotationMarkException final : public std::runtime_error {
public:
    ParseMissQuotationMarkException() : std::runtime_error("Missing quotation mark") {
    }
};

class ParseInvalidStringEscapeException final : public std::runtime_error {
public:
    ParseInvalidStringEscapeException() : std::runtime_error("Invalid string escape") {
    }
};

class ParseInvalidStringCharException final : public std::runtime_error {
public:
    ParseInvalidStringCharException() : std::runtime_error("Invalid string character") {
    }
};

class ParseInvalidUnicodeHexException final : public std::runtime_error {
public:
    ParseInvalidUnicodeHexException() : std::runtime_error("Invalid unicode hex") {
    }
};

class ParseInvalidUnicodeSurrogateException final : public std::runtime_error {
public:
    ParseInvalidUnicodeSurrogateException() : std::runtime_error("Invalid unicode surrogate") {
    }
};

class ParseMissCommaOrSquareBracketException final : public std::runtime_error {
public:
    ParseMissCommaOrSquareBracketException() : std::runtime_error("Missing comma or square bracket") {
    }
};

class ParseMissKeyException final : public std::runtime_error {
public:
    ParseMissKeyException() : std::runtime_error("Missing key") {
    }
};

class ParseMissColonException final : public std::runtime_error {
public:
    ParseMissColonException() : std::runtime_error("Missing colon") {
    }
};

class ParseMissCommaOrCurlyBracketException final : public std::runtime_error {
public:
    ParseMissCommaOrCurlyBracketException() : std::runtime_error("Missing comma or curly bracket") {
    }
};

// Helper function to throw the appropriate exception based on ParseException enum
inline void throwParseException(const ParseException e) {
    switch (e) {
        case ParseException::PARSE_EXPECT_VALUE:
            throw ParseExpectValueException();
        case ParseException::PARSE_INVALID_VALUE:
            throw ParseInvalidValueException();
        case ParseException::PARSE_ROOT_NOT_SINGULAR:
            throw ParseRootNotSingularException();
        case ParseException::PARSE_NUMBER_TOO_BIG:
            throw ParseNumberTooBigException();
        case ParseException::PARSE_MISS_QUOTATION_MARK:
            throw ParseMissQuotationMarkException();
        case ParseException::PARSE_INVALID_STRING_ESCAPE:
            throw ParseInvalidStringEscapeException();
        case ParseException::PARSE_INVALID_STRING_CHAR:
            throw ParseInvalidStringCharException();
        case ParseException::PARSE_INVALID_UNICODE_HEX:
            throw ParseInvalidUnicodeHexException();
        case ParseException::PARSE_INVALID_UNICODE_SURROGATE:
            throw ParseInvalidUnicodeSurrogateException();
        case ParseException::PARSE_MISS_COMMA_OR_SQUARE_BRACKET:
            throw ParseMissCommaOrSquareBracketException();
        case ParseException::PARSE_MISS_KEY:
            throw ParseMissKeyException();
        case ParseException::PARSE_MISS_COLON:
            throw ParseMissColonException();
        case ParseException::PARSE_MISS_COMMA_OR_CURLY_BRACKET:
            throw ParseMissCommaOrCurlyBracketException();
    }
}


class OutOfRangeException final : public std::runtime_error {
public:
    OutOfRangeException()
        : std::runtime_error("Index out of range") {
    }
};

class LiteralDisMatchException final : public std::runtime_error {
public:
    LiteralDisMatchException()
        : std::runtime_error("literal does not match") {
    }
};

#endif //EXCEPTION_H
