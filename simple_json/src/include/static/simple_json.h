//
// Created by mizuk on 2024/10/31.
//

#ifndef SIMPLE_JSON_H
#define SIMPLE_JSON_H
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <map>
#include "constant.h"
#include "exception.h"
#include "helper.h"

class JsonNode;

class Decoder {
private:
    std::vector<char> json;
    std::vector<char> stack;
    int cursor = 0;

    // property getters
    std::size_t size() const {
        return json.size();
    }

    bool hasNext() const {
        return cursor < json.size();
    }

    char ch() const {
        if (!hasNext()) {
            throw OutOfRangeException();
        }
        return json[cursor];
    }

    void resetStack() {
        this->stack.clear();
    }

    void forward() {
        this->putChar(this->ch());
        this->cursor++;
    }

    void putChar(const char c) {
        this->stack.push_back(c);
    }

    void trimWhiteSpace() {
        while (this->hasNext()
               && (this->ch() == ' ' || this->ch() == '\t' || this->ch() == '\n' || this->ch() == '\r')) {
            this->cursor++;
        }
    }

    std::shared_ptr<JsonNode> decodeLiteral(const std::string &literal);

    void validateNumber();

    std::shared_ptr<JsonNode> decodeNumber();

    std::shared_ptr<JsonNode> decodeString();

    std::shared_ptr<JsonNode> decodeArray();

    std::shared_ptr<JsonNode> decodeObject();

public:
    Decoder() = default;

    ~Decoder() = default;

    std::shared_ptr<JsonNode> decode(const std::string &input);

    // Optional: Add a convenience static method
    static std::shared_ptr<JsonNode> parse(const std::string &input);
};


using ArrayNode = std::vector<std::shared_ptr<JsonNode> >;
using ObjectNode = std::map<std::string, std::shared_ptr<JsonNode> >;

class JsonNode {
private:
    NodeType type;
    std::variant<
        std::monostate, // for null
        bool, // for true/false
        long double, // for numbers
        std::string, // for strings
        std::shared_ptr<ArrayNode>, // for arrays
        std::shared_ptr<ObjectNode> // for objects
    > value;

public:
    // Default constructor - creates null node
    JsonNode() : type(NodeType::Null), value(std::monostate{}) {
    }

    // Value constructors
    explicit JsonNode(bool b) : type(b ? NodeType::True : NodeType::False), value(b) {
    }

    explicit JsonNode(double d) : type(NodeType::Number), value(d) {
    }

    explicit JsonNode(const std::string &s) : type(NodeType::String), value(s) {
    }

    explicit JsonNode(std::string &&s) : type(NodeType::String), value(std::move(s)) {
    }

    // Array constructor
    explicit JsonNode(ArrayNode &&arr)
        : type(NodeType::Array)
          , value(std::make_shared<ArrayNode>(std::move(arr))) {
    }

    // Object constructor
    explicit JsonNode(ObjectNode &&obj)
        : type(NodeType::Object)
          , value(std::make_shared<ObjectNode>(std::move(obj))) {
    }

    // Getter for type
    NodeType getType() const { return type; }

    // Value getters
    bool getBool() const { return std::get<bool>(value); }
    auto getNumber() const { return std::get<long double>(value); }
    const std::string &getString() const { return std::get<std::string>(value); }
    const std::shared_ptr<ArrayNode> &getArray() const { return std::get<std::shared_ptr<ArrayNode> >(value); }
    const std::shared_ptr<ObjectNode> &getObject() const { return std::get<std::shared_ptr<ObjectNode> >(value); }

    // Copy and move operations
    JsonNode(const JsonNode &) = default;

    JsonNode &operator=(const JsonNode &) = default;

    JsonNode(JsonNode &&) noexcept = default;

    JsonNode &operator=(JsonNode &&) noexcept = default;

    ~JsonNode() = default;

    // Value setters
    void setNull() {
        type = NodeType::Null;
        value = std::monostate{};
    }

    void setBool(bool b) {
        type = b ? NodeType::True : NodeType::False;
        value = b;
    }

    void setNumber(long double d) {
        type = NodeType::Number;
        value = d;
    }

    void setString(const std::string &s) {
        type = NodeType::String;
        value = s;
    }

    void setString(std::string &&s) {
        type = NodeType::String;
        value = std::move(s);
    }

    void setArray(ArrayNode &&arr) {
        type = NodeType::Array;
        value = std::make_shared<ArrayNode>(std::move(arr));
    }

    void setObject(ObjectNode &&obj) {
        type = NodeType::Object;
        value = std::make_shared<ObjectNode>(std::move(obj));
    }

    // Convenience methods for array and object manipulation
    void addArrayElement(std::shared_ptr<JsonNode> element) {
        if (type != NodeType::Array) {
            type = NodeType::Array;
            value = std::make_shared<ArrayNode>();
        }
        std::get<std::shared_ptr<ArrayNode> >(value)->push_back(std::move(element));
    }

    void addObjectMember(const std::string &key, std::shared_ptr<JsonNode> element) {
        if (type != NodeType::Object) {
            type = NodeType::Object;
            value = std::make_shared<ObjectNode>();
        }
        std::get<std::shared_ptr<ObjectNode> >(value)->insert_or_assign(key, std::move(element));
    }
};

class Encoder {
public:
    Encoder() = default;

    ~Encoder() = default;

    std::string encode(const std::shared_ptr<JsonNode> &node);

    // Optional: Add a convenience static method
    static std::string stringify(const std::shared_ptr<JsonNode> &node);

private:
    std::string encodeString(const std::string &s);

    std::string encodeArray(const std::shared_ptr<ArrayNode> &arr);

    std::string encodeObject(const std::shared_ptr<ObjectNode> &obj);
};


#endif //SIMPLE_JSON_H
