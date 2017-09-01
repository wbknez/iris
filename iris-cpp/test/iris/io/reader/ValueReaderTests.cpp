#include <catch.hpp>

#include <sstream>
#include <string>

#include "iris/Types.hpp"
#include "iris/io/reader/ValueReader.hpp"

TEST_CASE("Test that values are parsed correctly.")
{
    SECTION("Verify correct values are read in as intended.")
    {
        using namespace iris;
        using namespace iris::io;
        using namespace std;

        auto iStream        = istringstream("2,3,2");
        const auto expected = ValueList{2, 3, 2};
        const auto result   = parseValuesFromCSV(iStream);

        CHECK(result.first  == expected);
        CHECK(result.second == expected);
    }

    SECTION("Verify correct handling of spaced values.")
    {
        using namespace iris;
        using namespace iris::io;
        using namespace std;

        auto iStream        = istringstream("2  ,   3   ,   2");
        const auto expected = ValueList{2, 3, 2};
        const auto result   = parseValuesFromCSV(iStream);

        CHECK(result.first  == expected);
        CHECK(result.second == expected);
    }

    SECTION("Verify correct values are read when two lines are used.")
    {
        using namespace iris;
        using namespace iris::io;
        using namespace std;

        auto iStream         = istringstream("2,3,2\n2,3,2");
        const auto result    = parseValuesFromCSV(iStream);

        const auto expected0 = ValueList{2, 3, 2};
        const auto expected1 = BehaviorList{2, 3, 2};
        
        CHECK(result.first  == expected0);
        CHECK(result.second == expected1);
    }

    SECTION("Verify bijectivity is enforced.")
    {
        using namespace iris;
        using namespace iris::io;
        using namespace std;

        auto iStream = istringstream("2,3,2\n1,3,2");
        CHECK_THROWS(parseValuesFromCSV(iStream));
    }

    SECTION("Verify the size requirement is enforced.")
    {
        using namespace iris;
        using namespace iris::io;
        using namespace std;

        auto iStream0 = istringstream("2,3,2\n2,3,2,4");
        CHECK_THROWS(parseValuesFromCSV(iStream0));

        auto iStream1 = istringstream("2,3,2,4\n2,3,2");
        CHECK_NOTHROW(parseValuesFromCSV(iStream1));
    }
}
