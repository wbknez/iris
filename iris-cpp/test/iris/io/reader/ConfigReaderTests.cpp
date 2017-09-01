#include <catch.hpp>

#include <sstream>
#include <string>

#include "iris/io/reader/ConfigReader.hpp"

TEST_CASE("Verify configuration files are parsed correctly.")
{
    SECTION("Verify basic parsing as intended.")
    {
        using namespace iris::io;
        using namespace std;

        auto iStream = istringstream("a = 3\nb = 4\nc = 0.3");
        auto result  = parseConfigurationFromCFG(iStream);

        CHECK(result["a"] == "3");
        CHECK(result["b"] == "4");
        CHECK(result["c"] == "0.3");
        CHECK(result.size() == 3);
    }

    SECTION("Verify parsing ignores comments.")
    {
        using namespace iris::io;
        using namespace std;

        auto iStream =
            istringstream("a = 3\n# This is a comment!\nb = 4\nc = 0.3");
        auto result =
            parseConfigurationFromCFG(iStream);
 
        CHECK(result["a"] == "3");
        CHECK(result["b"] == "4");
        CHECK(result["c"] == "0.3");
        CHECK(result.size() == 3);       
    }
}
