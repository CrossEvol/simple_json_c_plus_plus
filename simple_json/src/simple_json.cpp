//
// Created by mizuk on 2024/10/31.
//

#include <iostream>
#include <simple_json/simple_json.h>
#include "simple_json/helper.h"

std::shared_ptr<JsonNode> Decoder::decode() {
    this->trimWhiteSpace();
    if (!this->hasNext()) {
        throwParseException(ParseException::PARSE_EXPECT_VALUE);
    }
    // Implement actual parsing logic here
    switch (this->ch()) {
        case 'n':
            return decodeLiteral("null");
        case 't':
            return decodeLiteral("true");
        case 'f':
            return decodeLiteral("false");
        case '[':
            try {
                return decodeArray();
            } catch (OutOfRangeException &e) {
                throwParseException(ParseException::PARSE_MISS_COMMA_OR_SQUARE_BRACKET);
            }
        case '{':
            try {
                return decodeObject();
            } catch (OutOfRangeException &e) {
                throwParseException(ParseException::PARSE_MISS_COMMA_OR_CURLY_BRACKET);
            }
        case '"':
            return decodeString();
        default:
            return decodeNumber();
    }
}

std::shared_ptr<JsonNode> Decoder::decode(const std::string &input) {
    // Clear any existing state
    this->json.clear();
    this->stack.clear();
    this->cursor = 0;

    // Copy input string to json buffer
    this->json.assign(input.begin(), input.end());
    this->trimWhiteSpace();

    auto node = this->decode();
    this->trimWhiteSpace();
    if (this->hasNext()) {
        throwParseException(ParseException::PARSE_ROOT_NOT_SINGULAR);
    }
    return node;
}

std::shared_ptr<JsonNode> Decoder::decodeLiteral(const std::string &literal) {
    if (literal != +Literal::Null && literal != +Literal::True && literal != +Literal::False) {
        throw std::invalid_argument("literal does not match");
    }
    auto root = std::make_shared<JsonNode>();
    for (const char i: literal) {
        if (!hasNext()) {
            throwParseException(ParseException::PARSE_INVALID_VALUE);
        }
        if (this->ch() != i) {
            throwParseException(ParseException::PARSE_INVALID_VALUE);
        }
        this->skip();
    }

    if (literal == +Literal::Null) {
        root->setNull();
    } else if (literal == +Literal::True) {
        root->setBool(true);
    } else if (literal == +Literal::False) {
        root->setBool(false);
    }
    return root;
}

// Helper function to test string to double conversion
auto parseLongDouble = [](const std::string &str) {
    // Check for obviously too large exponents before conversion
    if (str.find('e') != std::string::npos || str.find('E') != std::string::npos) {
        auto e_pos = str.find('e') != std::string::npos ? str.find('e') : str.find('E');
        auto exp_str = str.substr(e_pos + 1);
        try {
            auto exp = std::stoi(exp_str);
            if (exp > 308) {
                // DBL_MAX_10_EXP is typically 308
                throwParseException(ParseException::PARSE_NUMBER_TOO_BIG);
            }
        } catch (ParseNumberTooBigException &pe) {
            throw;
        }
        catch (...) {
            throwParseException(ParseException::PARSE_INVALID_VALUE);
        }
    }

    try {
        return std::stold(str);
    } catch (const std::invalid_argument &e) {
        throwParseException(ParseException::PARSE_INVALID_VALUE);
    } catch (const std::out_of_range &e) {
        return static_cast<long double>(0.0);
    }
    return static_cast<long double>(0.0);
};

/*
 * this.forward() only used in first line of case condition
 * or inside the loop when case:DIGIT
 */
void Decoder::validateNumber() {
    this->resetStack();
    auto state{NumberState::BEGIN};
    auto hasDot{false};
    auto hasExponent{false};
    while (true) {
        switch (state) {
            case NumberState::BEGIN:
                if (this->ch() == '-') {
                    state = NumberState::NEGATIVE;
                } else if (this->ch() == '0') {
                    state = NumberState::ZERO;
                } else if (isDigit1to9(this->ch())) {
                    state = NumberState::DIGIT_1_TO_9;
                } else {
                    throwParseException(ParseException::PARSE_INVALID_VALUE);
                }
                break;
            case NumberState::NEGATIVE:
                this->forward();
                state = NumberState::DIGIT;
                break;
            case NumberState::ZERO:
                this->forward();
                if (this->ch() == '.') {
                    state = NumberState::DOT;
                } else if (this->ch() == 'e' || this->ch() == 'E') {
                    state = NumberState::EXPONENT;
                } else if (isWhiteSpace(this->ch())) {
                    // [ 0 ] or '0 x' , should return stack with '0'
                    return;
                } else {
                    return;
                }
                break;
            case NumberState::DIGIT:
                for (; isDigit(this->ch()); this->forward()) {
                }
                if (this->ch() == '.') {
                    if (hasDot) {
                        throwParseException(ParseException::PARSE_INVALID_VALUE);
                    }
                    state = NumberState::DOT;
                } else if (this->ch() == 'e' || this->ch() == 'E') {
                    if (hasExponent) {
                        throwParseException(ParseException::PARSE_INVALID_VALUE);
                    }
                    state = NumberState::EXPONENT;
                } else {
                    state = NumberState::END;
                }
                break;
            case NumberState::DIGIT_1_TO_9:
                this->forward();
                state = NumberState::DIGIT;
                break;
            case NumberState::DOT:
                hasDot = true;
                this->forward();
            // TODO: Is there any other method to verify the json has ended not by checking the OutOfRangeException?
                try {
                    if (isDigit(this->ch())) {
                        state = NumberState::DIGIT;
                    } else {
                        throwParseException(ParseException::PARSE_INVALID_VALUE);
                    }
                } catch (OutOfRangeException &e) {
                    throwParseException(ParseException::PARSE_INVALID_VALUE);
                }
                break;
            case NumberState::EXPONENT:
                hasExponent = true;
                this->forward();
                if (isDigit(this->ch())) {
                    state = NumberState::DIGIT;
                } else if (this->ch() == '+' || this->ch() == '-') {
                    state = NumberState::SYMBOL;
                } else {
                    throwParseException(ParseException::PARSE_INVALID_VALUE);
                }
                break;
            case NumberState::SYMBOL:
                this->forward();
                if (isDigit(this->ch())) {
                    state = NumberState::DIGIT;
                } else {
                    throwParseException(ParseException::PARSE_INVALID_VALUE);
                }
                break;
            case NumberState::END:
                return;
        }
    }
}

std::shared_ptr<JsonNode> Decoder::decodeNumber() {
    try {
        validateNumber();
    } catch (OutOfRangeException &e) {
        // TODO: use OutOfRangeException as the end signal
    }
    auto node = std::make_shared<JsonNode>();
    const std::string sv(this->stack.data(), this->stack.size());
    const auto number = parseLongDouble(sv);
    node->setNumber(number);
    return node;
}

std::shared_ptr<JsonNode> Decoder::decodeString() {
    auto node = std::make_shared<JsonNode>();

    if (this->ch() != '"') {
        throwParseException(ParseException::PARSE_MISS_QUOTATION_MARK);
    }
    this->skip();

    this->resetStack();

    for (;;) {
        if (!this->hasNext()) {
            throwParseException(ParseException::PARSE_MISS_QUOTATION_MARK);
        }
        switch (this->ch()) {
            case '"':
                this->skip();
                node->setString(std::string(this->stack.data(), this->stack.size()));
                return node;
            case '\\':
                this->skip();
                if (!this->hasNext()) {
                    throwParseException(ParseException::PARSE_INVALID_STRING_ESCAPE);
                }
                switch (this->ch()) {
                    case '"':
                        this->forward();
                        break;
                    case '\\':
                        this->forward();
                        break;
                    case '/':
                        this->forward();
                        break;
                    case 'b':
                        this->putChar('\b');
                        this->skip();
                        break;
                    case 'f':
                        this->putChar('\f');
                        this->skip();
                        break;
                    case 'n':
                        this->putChar('\n');
                        this->skip();
                        break;
                    case 'r':
                        this->putChar('\r');
                        this->skip();
                        break;
                    case 't':
                        this->putChar('\t');
                        this->skip();
                        break;
                    case 'u':
                        this->skip();
                        this->decodeUnicodeString();
                        break;
                    default:
                        throwParseException(ParseException::PARSE_INVALID_STRING_ESCAPE);
                        this->skip();
                }
                break;
            default:
                if (this->ch() < 0x20) {
                    throwParseException(ParseException::PARSE_INVALID_STRING_CHAR);
                }
                this->forward();
        }
    }
}

uint16_t Decoder::parseHex4() {
    uint16_t u = 0;
    for (int i = 0; i < 4; i++) {
        if (!hasNext()) {
            throwParseException(ParseException::PARSE_INVALID_UNICODE_HEX);
        }
        const char ch = this->ch();
        u <<= 4;
        if (ch >= '0' && ch <= '9') {
            u |= ch - '0';
        } else if (ch >= 'A' && ch <= 'F') {
            u |= ch - ('A' - 10);
        } else if (ch >= 'a' && ch <= 'f') {
            u |= ch - ('a' - 10);
        } else {
            throwParseException(ParseException::PARSE_INVALID_UNICODE_HEX);
        }
        this->skip();
    }
    return u;
}

void Decoder::decodeUnicodeString() {
    // Parse first UTF-16 code unit
    uint16_t u1 = parseHex4();

    // Handle surrogate pairs
    if (u1 >= 0xD800 && u1 <= 0xDBFF) {
        // Check if we have enough characters for surrogate pair
        if (!hasNext() || this->ch() != '\\') {
            throwParseException(ParseException::PARSE_INVALID_UNICODE_SURROGATE);
        }
        this->skip();
        if (!hasNext() || this->ch() != 'u') {
            throwParseException(ParseException::PARSE_INVALID_UNICODE_SURROGATE);
        }
        this->skip();

        // Parse second UTF-16 code unit
        uint16_t u2 = parseHex4();

        if (u2 < 0xDC00 || u2 > 0xDFFF) {
            throwParseException(ParseException::PARSE_INVALID_UNICODE_SURROGATE);
        }

        // Combine surrogate pair into single Unicode code point
        const uint32_t u = ((static_cast<uint32_t>(u1) - 0xD800) << 10 |
                            (static_cast<uint32_t>(u2) - 0xDC00)) + 0x10000;

        // Encode as UTF-8
        this->encodeUTF8(u);
    } else {
        // Encode single Unicode character
        this->encodeUTF8(u1);
    }
}

// Helper method to encode Unicode code points as UTF-8
void Decoder::encodeUTF8(const uint32_t u) {
    if (u <= 0x7F) {
        // 1-byte sequence
        this->stack.push_back(static_cast<char>(u & 0xFF));
    } else if (u <= 0x7FF) {
        // 2-byte sequence
        this->stack.push_back(static_cast<char>(0xC0 | ((u >> 6) & 0xFF)));
        this->stack.push_back(static_cast<char>(0x80 | (u & 0x3F)));
    } else if (u <= 0xFFFF) {
        // 3-byte sequence
        this->stack.push_back(static_cast<char>(0xE0 | ((u >> 12) & 0xFF)));
        this->stack.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3F)));
        this->stack.push_back(static_cast<char>(0x80 | (u & 0x3F)));
    } else if (u <= 0x10FFFF) {
        // 4-byte sequence
        this->stack.push_back(static_cast<char>(0xF0 | ((u >> 18) & 0xFF)));
        this->stack.push_back(static_cast<char>(0x80 | ((u >> 12) & 0x3F)));
        this->stack.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3F)));
        this->stack.push_back(static_cast<char>(0x80 | (u & 0x3F)));
    } else {
        throwParseException(ParseException::PARSE_INVALID_UNICODE_HEX);
    }
}

std::shared_ptr<JsonNode> Decoder::decodeArray() {
    auto node = std::make_shared<JsonNode>();
    if (this->ch() != '[') {
        throwParseException(ParseException::PARSE_MISS_COMMA_OR_SQUARE_BRACKET);
    }
    this->skip();
    this->trimWhiteSpace();
    if (this->ch() == ']') {
        this->skip();
        ArrayNode array{};
        node->setArray(std::move(array));
        return node;
    }
    ArrayNode array{};
    for (;;) {
        if (auto subNode = this->decode(); subNode != nullptr) {
            this->trimWhiteSpace();
            array.push_back(subNode);
        }
        if (this->ch() == ',') {
            this->skip();
            this->trimWhiteSpace();
        } else if (this->ch() == ']') {
            this->skip();
            node->setArray(std::move(array));
            return node;
        } else {
            throwParseException(ParseException::PARSE_MISS_COMMA_OR_SQUARE_BRACKET);
        }
    }
}

std::shared_ptr<JsonNode> Decoder::decodeObject() {
    auto node = std::make_shared<JsonNode>();

    if (this->ch() != '{') {
        throwParseException(ParseException::PARSE_MISS_COMMA_OR_CURLY_BRACKET);
    }
    this->skip();
    this->trimWhiteSpace();

    if (this->ch() == '}') {
        this->skip();
        ObjectNode object{};
        node->setObject(std::move(object));
        return node;
    }

    ObjectNode object{};
    for (;;) {
        if (this->ch() != '"') {
            throwParseException(ParseException::PARSE_MISS_KEY);
        }
        auto keyNode = this->decodeString();
        if (keyNode == nullptr) {
            throwParseException(ParseException::PARSE_INVALID_VALUE);
        }
        this->trimWhiteSpace();
        if (this->ch() != ':') {
            throwParseException(ParseException::PARSE_MISS_COLON);
        }
        this->skip();
        this->trimWhiteSpace();
        auto valueNode = this->decode();
        if (valueNode == nullptr) {
            throwParseException(ParseException::PARSE_INVALID_VALUE);
        }
        object.emplace(keyNode->getString(), std::move(valueNode));
        this->trimWhiteSpace();
        if (this->ch() == ',') {
            this->skip();
            this->trimWhiteSpace();
            if (!this->hasNext()) {
                throwParseException(ParseException::PARSE_MISS_KEY);
            }
        } else if (this->ch() == '}') {
            this->skip();
            node->setObject(std::move(object));
            return node;
        } else {
            throwParseException(ParseException::PARSE_MISS_COMMA_OR_CURLY_BRACKET);
            break;
        }
    }
    return node;
}

// Optional: Add a convenience static method
std::shared_ptr<JsonNode> Decoder::parse(const std::string &input) {
    Decoder decoder;
    return decoder.decode(input);
}

std::string Encoder::encode(const std::shared_ptr<JsonNode> &node) {
    // TODO: Implement string encoding
    // Should handle escape sequences and special characters
    return "";
}

std::string Encoder::encodeString(const std::string &s) {
    // TODO: Implement string encoding
    // Should handle escape sequences and special characters
    return "";
}

std::string Encoder::encodeArray(const std::shared_ptr<ArrayNode> &arr) {
    // TODO: Implement array encoding
    // Should handle nested elements recursively
    return "";
}

std::string Encoder::encodeObject(const std::shared_ptr<ObjectNode> &obj) {
    // TODO: Implement object encoding
    // Should handle key-value pairs recursively
    return "";
}

// Optional: Add a convenience static method
std::string Encoder::stringify(const std::shared_ptr<JsonNode> &node) {
    Encoder encoder;
    return encoder.encode(node);
}
