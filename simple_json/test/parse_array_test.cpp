#include <catch2/catch_test_macros.hpp>
#include <simple_json/simple_json.h>

// Helper function to test array parsing
void TEST_ARRAY(const std::string &json, const std::vector<NodeType> &expectedTypes) {
    const auto node = Decoder::parse(json);
    REQUIRE(node->getType() == NodeType::Array);
    const auto &array = node->getArray();
    REQUIRE(array->size() == expectedTypes.size());

    for (size_t i = 0; i < expectedTypes.size(); i++) {
        REQUIRE((*array)[i]->getType() == expectedTypes[i]);
    }
}

TEST_CASE("Parse array tests", "[array]") {
    SECTION("Empty array") {
        const auto node = Decoder::parse("[ ]");
        REQUIRE(node->getType() == NodeType::Array);
        REQUIRE(node->getArray()->empty());
    }

    SECTION("Array with mixed types") {
        TEST_ARRAY("[ null , false , true , 123 , \"abc\" ]", {
                       NodeType::Null,
                       NodeType::False,
                       NodeType::True,
                       NodeType::Number,
                       NodeType::String
                   });

        // Additional verification of values
        const auto node = Decoder::parse("[ null , false , true , 123 , \"abc\" ]");
        const auto &array = node->getArray();

        REQUIRE((*array)[3]->getNumber() == 123.0);
        REQUIRE((*array)[4]->getString() == "abc");
    }

    SECTION("Nested arrays") {
        const auto node = Decoder::parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]");
        REQUIRE(node->getType() == NodeType::Array);
        const auto &outerArray = node->getArray();
        REQUIRE(outerArray->size() == 4);

        for (size_t i = 0; i < 4; i++) {
            const auto &innerArray = (*outerArray)[i];
            REQUIRE(innerArray->getType() == NodeType::Array);
            REQUIRE(innerArray->getArray()->size() == i);

            for (size_t j = 0; j < i; j++) {
                const auto &element = (*innerArray->getArray())[j];
                REQUIRE(element->getType() == NodeType::Number);
                REQUIRE(element->getNumber() == static_cast<long double>(j));
            }
        }
    }
}
