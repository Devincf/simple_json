#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "../catch2/catch.hpp"

#include "../../include/simple_json.hpp"
#include <string>

/*
unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
}
*/


TEST_CASE( "Json Object created"){
    simple_json::json object;

    //int test
    int var_int = 1;
    object.insert("int", var_int);
    CHECK( (int) object["int"] == 1);
    object["int"] = 3;
    CHECK( (int) object["int"] == 3);

    //double test
    double var_double = 1.3;
    object.insert("double", var_double);
    CHECK( (double) object["double"] == 1.3);
    object["double"] = 3.33333;
    CHECK( (double) object["double"] == 3.33333);

    //bool test
    bool var_bool = true;
    object.insert("bool", var_bool);
    CHECK( (bool) object["bool"]);
    object["bool"] = false;
    CHECK( !(bool) object["bool"]);

    //string test
    std::string var_string = "test string";
    object.insert("string", var_string);
    CHECK( (std::string) object["string"] == "test string");
    object["string"] = "after modification";
    CHECK( (std::string) object["string"] == "after modification");

    deb("type checks done")

    //simple object test
    simple_json::json another = object;

    object.insert("object", another);

    another["int"] = 1111;
    object["object"]["int"] = 8888;

    deb(object.to_str())
}