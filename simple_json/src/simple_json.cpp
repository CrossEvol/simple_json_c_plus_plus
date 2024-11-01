//
// Created by mizuk on 2024/10/31.
//

#include <iostream>
#include <static/simple_json.h>
#include "static/helper.h"

std::shared_ptr<JsonNode> Decoder::decode(const std::string &input) {
    // Clear any existing state
    json.clear();
    stack.clear();
    cursor = 0;

    // Copy input string to json buffer
    json.assign(input.begin(), input.end());

    // Implement actual parsing logic here
    switch (json[cursor]) {
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
    auto root = std::make_shared<JsonNode>();
    return root;
}

std::shared_ptr<JsonNode> Decoder::decodeArray() {
    auto root = std::make_shared<JsonNode>();
    return root;
}

std::shared_ptr<JsonNode> Decoder::decodeObject() {
    auto root = std::make_shared<JsonNode>();
    return root;
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
