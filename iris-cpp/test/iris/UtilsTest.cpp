#include <catch.hpp>

#include <stdexcept>
#include <string>
#include <vector>

#include "iris/Types.hpp"
#include "iris/Utils.hpp"

typedef std::vector<iris::AgentID> IdVector;

TEST_CASE("Ensure binary search works correctly.")
{
    IdVector network{2, 3, 5, 7, 8, 10, 15, 53, 69, 77};
    
    SECTION("Searching for a non-existent element returns the end.")
    {
        using namespace iris::util;
        
        const auto loc = binarySearch(network.begin(), network.end(), 100);        
        CHECK(loc == network.end());
        CHECK((loc - network.begin()) >= network.size());
    }
    
    SECTION("Searching for a specific element returns correct location.")
    {
        using namespace iris::util;
        
        const auto loc = binarySearch(network.begin(), network.end(), 7);
        CHECK((loc - network.begin()) == 3);
    }
}

TEST_CASE("Exploring the lower sphere of the sample space.")
{
    IdVector excludes{0, 1, 2, 4, 6};

    SECTION("Going downwards on filled surface goes out of bounds.")
    {
        using namespace iris;
        using namespace iris::types;
        using namespace iris::util;
        
        const auto val = exploreLowerSphere<AgentID>(2, 2, excludes);
        CHECK(val == (uint32)-1);
    }

    SECTION("Going downwards towards available targets finds a hole.")
    {
        using namespace iris;
        using namespace iris::util;
        
        const auto val = exploreLowerSphere<AgentID>(4, 3, excludes);
        CHECK(val == 3);
    }

    SECTION("Starting in a hole returns it.")
    {
        using namespace iris;
        using namespace iris::util;
        
        const auto val = exploreLowerSphere<AgentID>(5, 4, excludes);
        CHECK(val == 5);
    }
}

TEST_CASE("Exploring the upper sphere of the sample space.")
{
    IdVector excludes{0, 2, 4, 5, 6};

    SECTION("Going upwards on filled surface goes out of bounds.")
    {
        using namespace iris;
        using namespace iris::util;
        
        const auto val = exploreUpperSphere<AgentID>(4, 0, excludes);
        CHECK(val == 7);
    }

    SECTION("Going upwards towards available targets finds a hole.")
    {
        using namespace iris;
        using namespace iris::util;
        
        const auto val = exploreUpperSphere<AgentID>(2, 0, excludes);
        CHECK(val == 3);
    }

    SECTION("Starting in a hole returns it.")
    {
        using namespace iris;
        using namespace iris::util;
        
        const auto val = exploreUpperSphere<AgentID>(3, 0, excludes);
        CHECK(val == 3);
    }
}

TEST_CASE("Ensuring the uniqueness of random numbers.")
{
    IdVector excludes{0, 2, 3, 6, 8, 11, 12, 23, 45, 76, 77, 78};
    IdVector full{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    SECTION("Going out of range below correctly throws.")
    {
        using namespace iris;
        using namespace iris::util;
        
        CHECK_THROWS(ensureRandom<AgentID>((AgentID)0, full,
                                          (AgentID)0, (AgentID)10));
    }

    SECTION("Going out of range above correctly throws.")
    {
        using namespace iris;
        using namespace iris::util;

        CHECK_THROWS(ensureRandom<AgentID>((AgentID)9, full,
                                           (AgentID)0, (AgentID)10));
    }

    SECTION("Correctly finding a hole behind going forward.")
    {
        using namespace iris;
        using namespace iris::util;

        const auto val =
            ensureRandom<AgentID>((AgentID)76, excludes,
                                  (AgentID)0, (AgentID)10);
        CHECK(val == 75);
    }
}

TEST_CASE("Test that sorted insert works correctly.")
{
    IdVector sample{1, 3, 5, 6, 8, 9, 10};

    SECTION("Insert correctly between two values.")
    {
        using namespace iris;
        using namespace iris::util;

        IdVector clone(sample);
        IdVector expected{1, 3, 5, 6, 7, 8, 9, 10};
        
        sortedInsert(clone, (AgentID)7);
        CHECK(expected == clone);
    }

    SECTION("Insert correctly at front of list.")
    {
        using namespace iris;
        using namespace iris::util;

        IdVector clone(sample);
        IdVector expected{0, 1, 3, 5, 6, 8, 9, 10};

        sortedInsert(clone, (AgentID)0);
        CHECK(expected == clone);
    }

    SECTION("Insert correctly at back of list.")
    {
        using namespace iris;
        using namespace iris::util;

        IdVector clone(sample);
        IdVector expected{1, 3, 5, 6, 8, 9, 10, 12};

        sortedInsert(clone, (AgentID)12);
        CHECK(expected == clone);        
    }
}

TEST_CASE("Quick test of trim.")
{
    SECTION("Verify trimming actually occurs.")
    {
        using namespace iris::util;
        using namespace std;

        auto trimmable = string("    I am a trimmable string!   !   ");
        auto expected  = string("I am a trimmable string!   !");

        trimmable = trim(trimmable);
        CHECK(expected == trimmable);
    }
}

TEST_CASE("Ensure conversion to strings works correctly.")
{
    SECTION("Test long conversion.")
    {
        using namespace iris::types;
        using  namespace iris::util;
        using namespace std;
        
        const auto expected0 = string("10");
        const auto expected1 = string("100");
        const auto expected2 = string("1000");

        const auto result0   = toString<uint64>(10);
        const auto result1   = toString<uint64>(100);
        const auto result2   = toString<uint64>(1000);

        CHECK(result0 == expected0);
        CHECK(result1 == expected1);
        CHECK(result2 == expected2);

        // Check for commas.
        const auto comma0    = result0.find(",");
        const auto comma1    = result1.find(",");
        const auto comma2    = result2.find(",");

        CHECK(comma0 == string::npos);
        CHECK(comma1 == string::npos);
        CHECK(comma2 == string::npos);
    }
}
