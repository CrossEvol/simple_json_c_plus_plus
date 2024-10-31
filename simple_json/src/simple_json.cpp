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

    // Create root node
    auto root = std::make_shared<JsonNode>();

    // TODO: Implement actual parsing logic here
    // This will be implemented in subsequent steps
    // For now, just return an empty node

    return root;
}

// Optional: Add a convenience static method
std::shared_ptr<JsonNode> Decoder::parse(const std::string &input) {
    Decoder decoder;
    return decoder.decode(input);
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
