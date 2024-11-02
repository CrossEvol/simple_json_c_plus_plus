//
// Created by mizuk on 2024/10/31.
//

#include <iostream>
#include <static/simple_json.h>
#include "static/helper.h"

std::shared_ptr<JsonNode> Decoder::decode() {
    // Implement actual parsing logic here
    switch (this->ch()) {
        case 'n':
            return decodeLiteral("null");
        case 't':
            return decodeLiteral("true");
        case 'f':
            return decodeLiteral("false");
        case '[':
            return decodeArray();
        case '{':
            return decodeObject();
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
            throw OutOfRangeException();
        }
        if (this->ch() != i) {
            throwParseException(ParseException::PARSE_INVALID_VALUE);
        }
        this->cursor++;
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
    try {
        return std::stold(str);
    } catch (const std::invalid_argument &e) {
        std::cout << e.what() << std::endl;
        throwParseException(ParseException::PARSE_INVALID_VALUE);
    } catch (const std::out_of_range &e) {
        std::cout << e.what() << std::endl;
        throwParseException(ParseException::PARSE_NUMBER_TOO_BIG);
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
                    throwParseException(ParseException::PARSE_INVALID_VALUE);
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
                if (isDigit(this->ch())) {
                    state = NumberState::DIGIT;
                } else {
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
    } catch (OutOfRangeException e) {
        // TODO: use OutOfRangeException as the end signal
        // std::cout << e.what() << std::endl;
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
                    throwParseException(ParseException::PARSE_INVALID_STRING_ESCAPE);
                }
                this->forward();
        }
    }
}

void Decoder::decodeUnicodeString() {
    // Check if we have enough characters for the first unicode hex (4 digits)
    if (this->cursor + 4 >= this->size()) {
        throwParseException(ParseException::PARSE_INVALID_UNICODE_HEX);
    }

    // Parse first UTF-16 code unit
    const std::string u1Str(json.begin() + cursor, json.begin() + cursor + 4);
    uint16_t u1 = 0;
    try {
        u1 = static_cast<uint16_t>(std::stoul(u1Str, nullptr, 16));
    } catch (...) {
        throwParseException(ParseException::PARSE_INVALID_UNICODE_HEX);
    }
    cursor += 4; // Skip 'u' and 4 hex digits

    // Handle surrogate pairs
    if (u1 >= 0xD800 && u1 <= 0xDBFF) {
        // Check if we have enough characters for surrogate pair
        if (cursor + 6 >= json.size() ||
            json[cursor] != '\\' ||
            json[cursor + 1] != 'u') {
            throwParseException(ParseException::PARSE_INVALID_UNICODE_SURROGATE);
        }

        // Parse second UTF-16 code unit
        const std::string u2Str(json.begin() + cursor + 2, json.begin() + cursor + 6);
        uint16_t u2 = 0;
        try {
            u2 = static_cast<uint16_t>(std::stoul(u2Str, nullptr, 16));
        } catch (...) {
            throwParseException(ParseException::PARSE_INVALID_UNICODE_HEX);
        }

        if (u2 < 0xDC00 || u2 > 0xDFFF) {
            throwParseException(ParseException::PARSE_INVALID_UNICODE_SURROGATE);
        }

        // Combine surrogate pair into single Unicode code point
        const uint32_t u = ((static_cast<uint32_t>(u1) - 0xD800) << 10 |
                            (static_cast<uint32_t>(u2) - 0xDC00)) + 0x10000;

        // Encode as UTF-8
        this->encodeUTF8(u);
        cursor += 6;
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
