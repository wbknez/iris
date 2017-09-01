#include <catch.hpp>

#include <random>

#include "iris/Agent.hpp"
#include "iris/Types.hpp"

#include "iris/gen/AttributeGenerator.hpp"

iris::types::uint32 getPowerfulAgentCount(iris::Agent* agents,
                                          iris::AgentID totalAgents)
{
    using namespace iris;
    using namespace iris::types;
    
    uint32 counter = 0;

    for(auto i = (AgentID)0; i < totalAgents; i++)
    {
        if(agents[i].isPowerful())
        {
            counter++;
        }
    }

    return counter;
}

TEST_CASE("Verify that conversion between Uint32Lists and integer"
          " types is correct.")
{
    SECTION("Verify that conversion works correctly.")
    {
        using namespace iris;
        using namespace iris::gen;
        using namespace iris::types;

        const auto list     = Uint32List{1, 2, 3, 2, 1};
        const auto expected = (uint32)(12321);
        const auto result   = convertListToInteger(list);

        CHECK(result == expected);
    }

    SECTION("Verify that zero is handled correctly.")
    {
        using namespace iris;
        using namespace iris::gen;
        using namespace iris::types;

        const auto list     = Uint32List{0};
        const auto expected = (uint32)0;
        const auto result   = convertListToInteger(list);

        CHECK(result == expected);
    }
}

TEST_CASE("Verify that conversion between Uint32List and std::string"
          " is correct.")
{
    SECTION("Verify that conversion works correctly.")
    {
        using namespace iris;
        using namespace iris::gen;
        using namespace iris::types;

        const auto list     = Uint32List{1, 2, 3, 2, 1};
        const auto expected = std::string("12321");
        const auto result   = convertListToString(list);

        CHECK(result == expected);        
    }
}

TEST_CASE("Verify that population dispensers are being created correctly.")
{
    SECTION("Create simple dispenser.")
    {
        using namespace iris;
        using namespace iris::gen;
        using namespace iris::types;
        using namespace std;

        random_device    seed;
        mersenne_twister random(seed());
        
        const auto variables = Uint32List{2};
        auto       popDist   = Uint32List(2);
        auto       result    = createPopulationDispensers(variables, 100);

        while(result[0].hasMore())
        {
            const auto next = result[0].nextGroup(random);
            popDist[next] += 1;
        }

        CHECK(popDist[0] == popDist[1]);
        CHECK(popDist[0] == 50);
        CHECK(popDist[1] == 50);
    }
}

TEST_CASE("Verify that attribute lists are being created correctly.")
{
    SECTION("Simple test case.")
    {
        using namespace iris;
        using namespace iris::gen;
        using namespace iris::types;
        using namespace std;

        random_device    seed;
        mersenne_twister random(seed());
        
        const auto variables = Uint32List{2};
        auto       popDisp   = createPopulationDispensers(variables, 2);
        
        const auto result0   = createAttributeList(popDisp, random);
        const auto result1   = createAttributeList(popDisp, random);

        CHECK(result0 != result1);

        if(result0[0] == 0)
        {
            CHECK(result0[0] == 0);
            CHECK(result1[0] == 1); 
        }
        else
        {
            CHECK(result0[0] == 1);
            CHECK(result1[0] == 0);
        }
    }
}

TEST_CASE("Verify that sub-lists are created and copied correctly.")
{
    SECTION("Simple copy test case.")
    {
        using namespace iris;
        using namespace iris::gen;

        const auto values    = Uint32List{2, 3, 4};
        const auto behaviors = Uint32List{2, 3, 4};
        const auto expected  = Uint32List{2, 3, 4};
        const auto result    = createSubAttributeList(values, behaviors);

        CHECK(result == expected);
    }

    SECTION("Simple copy test case using single digits.")
    {
        using namespace iris;
        using namespace iris::gen;

        const auto values    = Uint32List{2};
        const auto behaviors = Uint32List{2};
        const auto expected  = Uint32List{2};
        const auto result    = createSubAttributeList(values, behaviors);

        CHECK(result == expected);
    }

    SECTION("Test when desired list is an actual sub-list (less in length).")
    {
        using namespace iris;
        using namespace iris::gen;

        const auto values    = Uint32List{2, 3, 4};
        const auto behaviors = Uint32List{2, 3};
        const auto expected  = Uint32List{2, 3};
        const auto result    = createSubAttributeList(values, behaviors);

        CHECK(result == expected);
    }

    SECTION("Test when desired list is an actual sub-list (less in length)"
            " for single digits.")
    {
        using namespace iris;
        using namespace iris::gen;

        const auto values    = Uint32List{2, 3};
        const auto behaviors = Uint32List{2};
        const auto expected  = Uint32List{2};
        const auto result    = createSubAttributeList(values, behaviors);

        CHECK(result == expected);
    }
}

TEST_CASE("Verify that powerful agents are created correctly.")
{
    using namespace iris;
    using namespace iris::gen;
    using namespace iris::types;
    using namespace std;

    random_device    seed;
    mersenne_twister random(seed());

    auto             agents = new Agent[20];

    for(auto i = 0; i < 20; i++)
    {
        agents[i].setPowerful(false);
        agents[i].setUId(i);
    }
    
    SECTION("Correctly handle a power percentage of zero.")
    {
        generatePowerfulAgents(agents, (AgentID)20, (fnumeric)0, true, random);
        
        const auto count = getPowerfulAgentCount(agents, (AgentID)20);
        CHECK(count == 0);
    }

    SECTION("Correctly handle a power percentage of ten.")
    {
        generatePowerfulAgents(agents, (AgentID)20, 0.10, true, random);
        
        const auto count = getPowerfulAgentCount(agents, (AgentID)20);
        CHECK(count == 2);
    }

    SECTION("Correctly handle floor calculation.")
    {
        generatePowerfulAgents(agents, (AgentID)20, 0.16, true, random);

        const auto count = getPowerfulAgentCount(agents, (AgentID)20);
        CHECK(count == 3);
    }

    SECTION("Correctly handle requiring at least one.")
    {
        generatePowerfulAgents(agents, (AgentID)20, 0.01, true, random);

        const auto count = getPowerfulAgentCount(agents, (AgentID)20);
        CHECK(count == 1);
    }

    SECTION("Correctly handle not requiring at least one.")
    {
        generatePowerfulAgents(agents, (AgentID)20, 0.01, false, random);

        const auto count = getPowerfulAgentCount(agents, (AgentID)20);
        CHECK(count == 0);
    }

    delete[] agents;
}

TEST_CASE("Ensure that attribute list permutation works correctly.")
{
    SECTION("Verify simple permutation.")
    {
        using namespace iris;
        using namespace iris::gen;
        
        const auto vars     = Uint32List{2,2};
        const auto expected = PermuteList{"00", "01", "10", "11"};
        const auto result   = permuteList(vars);

        CHECK(result.size() == expected.size());
        CHECK(result == expected);
    }

    SECTION("Verify order of uneven permutation(s).")
    {
        using namespace iris;
        using namespace iris::gen;
        
        const auto vars     = Uint32List{2, 3, 2};
        const auto expected = PermuteList{
            "000", "001",
            "010", "011",
            "020", "021",
            "100", "101",
            "110", "111",
            "120", "121",
        };
        const auto result   = permuteList(vars);

        CHECK(result.size() == expected.size());
        CHECK(result == expected);        
    }
}

TEST_CASE("Verify that attribution generation works correctly.")
{
    using namespace iris;
    using namespace iris::gen;
    using namespace iris::types;
    using namespace std;

    random_device    seed;
    mersenne_twister random(seed());

    auto agents = new Agent[2];

    SECTION("Check that behaviors are assigned as correct sub-lists when"
            " values and behavior sizes are equal.")
    {
        const auto values    = Uint32List{2};
        const auto behaviors = Uint32List{2};

        generateAttributes(agents, 2, values, behaviors, random);

        const auto behav0 = agents[0].getBehavior();
        const auto behav1 = agents[1].getBehavior();

        CHECK(behav0 != behav1);

        if(behav0[0] == 0)
        {
            CHECK(behav0[0] == 0);
            CHECK(behav1[0] == 1);
        }
        else
        {
            CHECK(behav0[0] == 1);
            CHECK(behav1[0] == 0);
        }
    }
    
    delete[] agents;
}
