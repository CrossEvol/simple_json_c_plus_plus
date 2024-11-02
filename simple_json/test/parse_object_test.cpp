#include <catch2/catch_test_macros.hpp>
#include <static/simple_json.h>

// Helper function to test object parsing
void TEST_OBJECT(const std::string& json, const std::map<std::string, NodeType>& expectedTypes) {
    const auto node = Decoder::parse(json);
    REQUIRE(node->getType() == NodeType::Object);
    const auto& obj = node->getObject();
    REQUIRE(obj->size() == expectedTypes.size());

    for (const auto& [key, expectedType] : expectedTypes) {
        REQUIRE(obj->count(key) > 0);
        REQUIRE((*obj)[key]->getType() == expectedType);
    }
}

TEST_CASE("Parse object tests", "[object]") {
    SECTION("Empty object") {
        const auto node = Decoder::parse("{ }");
        REQUIRE(node->getType() == NodeType::Object);
        REQUIRE(node->getObject()->empty());
    }

    SECTION("Object with mixed types") {
        const std::string json = R"({
            "n": null,
            "f": false,
            "t": true,
            "i": 123,
            "s": "abc",
            "a": [1, 2, 3],
            "o": {"1": 1, "2": 2, "3": 3}
        })";

        const auto node = Decoder::parse(json);
        REQUIRE(node->getType() == NodeType::Object);
        const auto& obj = node->getObject();
        REQUIRE(obj->size() == 7);

        // Test individual members
        REQUIRE((*obj)["n"]->getType() == NodeType::Null);
        REQUIRE((*obj)["f"]->getType() == NodeType::False);
        REQUIRE((*obj)["t"]->getType() == NodeType::True);

        REQUIRE((*obj)["i"]->getType() == NodeType::Number);
        REQUIRE((*obj)["i"]->getNumber() == 123.0);

        REQUIRE((*obj)["s"]->getType() == NodeType::String);
        REQUIRE((*obj)["s"]->getString() == "abc");

        // Test array member
        const auto& array = (*obj)["a"];
        REQUIRE(array->getType() == NodeType::Array);
        const auto& arrayValues = array->getArray();
        REQUIRE(arrayValues->size() == 3);
        for (size_t i = 0; i < 3; i++) {
            REQUIRE((*arrayValues)[i]->getType() == NodeType::Number);
            REQUIRE((*arrayValues)[i]->getNumber() == i + 1.0);
        }

        // Test nested object
        const auto& nestedObj = (*obj)["o"];
        REQUIRE(nestedObj->getType() == NodeType::Object);
        const auto& nestedObjValues = nestedObj->getObject();
        REQUIRE(nestedObjValues->size() == 3);
        for (size_t i = 1; i <= 3; i++) {
            const std::string key = std::to_string(i);
            REQUIRE((*nestedObjValues)[key]->getType() == NodeType::Number);
            REQUIRE((*nestedObjValues)[key]->getNumber() == static_cast<double>(i));
        }
    }
}

