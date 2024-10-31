//
// Created by mizuk on 2024/10/31.
//

#include <static/simple_json.h>

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

std::shared_ptr<JsonNode> Decoder::decodeNumber() {
    auto root = std::make_shared<JsonNode>();
    return root;
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
