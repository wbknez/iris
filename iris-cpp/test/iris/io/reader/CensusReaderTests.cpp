#include <catch.hpp>

#include <sstream>
#include <string>

#include "iris/io/reader/CensusReader.hpp"

TEST_CASE("Verify the census reader reads in fnumerics correctly.")
{
    SECTION("Verify census data is parsed correctly as intended.")
    {
        using namespace iris::io;
        using namespace std;
        
        auto iStream        = istringstream("0.3,0.4,0.12,0.19");
        const auto expected = CensusData{0.3, 0.4, 0.12, 0.19};
        const auto result   = parseCensusDataFromCSV(iStream);

        CHECK(expected == result);
    }

    SECTION("Verify census data is parsed despite odd spacing.")
    {
        using namespace iris::io;
        using namespace std;
        
        auto iStream        = istringstream("     0.3  ,  0.4,   0.12   ,0.19 ");
        const auto expected = CensusData{0.3, 0.4, 0.12, 0.19};
        const auto result   = parseCensusDataFromCSV(iStream);

        CHECK(expected == result);
    }
}
