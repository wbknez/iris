#include <catch.hpp>

#include <algorithm>
#include <random>
#include <vector>

#include "iris/Agent.hpp"
#include "iris/Types.hpp"

#include "iris/gen/GraphGenerator.hpp"

#include "iris/io/reader/CensusReader.hpp"

void testForDuplicates(const iris::Agent::Network& network)
{
    if(network.size() == 0)
    {
        return;
    }
    
    for(iris::Agent::Network::size_type i = 0; i < network.size() - 1; i++)
    {
        CHECK(network[i] != network[i + 1]);
    }
}

void testForLoops(const iris::AgentID& id, const iris::Agent::Network& network)
{
    for(iris::Agent::Network::size_type i = 0; i < network.size(); i++)
    {
        CHECK(network[i] != id);
    }
}

TEST_CASE("Verify family size selection works correctly.")
{
    SECTION("Verify singular selection.")
    {
        using namespace iris::gen;

        const auto cdf     = CDF{1.0};
        const auto result0 = chooseFamilySize(0.2, cdf);
        const auto result1 = chooseFamilySize(0.8, cdf);

        CHECK(result0 == 0);
        CHECK(result1 == 0);
    }

    SECTION("Verify multiple selection.")
    {
        using namespace iris::gen;

        const auto cdf     = CDF{0.2, 0.5, 0.7, 1.0};
        const auto result0 = chooseFamilySize(0.3, cdf);
        const auto result1 = chooseFamilySize(0.7, cdf);

        CHECK(result0 == 1);
        CHECK(result1 == 2);
    }

    SECTION("Verify throws when out of bounds.")
    {
        using namespace iris::gen;
        
        const auto cdf = CDF{0.2, 0.5, 0.7, 1.0};
        CHECK_THROWS(chooseFamilySize(1.1, cdf));
    }
}

TEST_CASE("Verify CDF creation works as expected.")
{
    SECTION("Verify construction from census data.")
    {
        using namespace iris::gen;
        using namespace iris::io;

        const auto census   = CensusData{0.2, 0.1, 0.4, 0.2, 0.1};
        const auto expected = CDF{0.2, 0.3, 0.7, 0.9, 1.0};
        const auto result   = createCDF(census);

        REQUIRE(expected.size() == result.size());

        SECTION("Verify the results and expected are equal within epsilon.")
        {
            for(CDF::size_type i = 0; i < result.size(); i++)
            {
                CHECK(result[i] == Approx(expected[i]));
            }
        }
    }
}

TEST_CASE("Wire a family unit together.")
{
    SECTION("Verify that a single unit in a family is wired.")
    {
        using namespace iris;
        using namespace iris::gen;

        Agent      agent;
        const auto familyUnit = FamilyUnit{0, 4};

        agent.setUId(0);
        wireFamilyUnit(agent, familyUnit);

        const auto expected = Agent::Network{1, 2, 3};
        const auto result   = agent.getNetwork();
        
        CHECK(expected == result);
    }

    SECTION("Verify that all units in a family are wired.")
    {
        using namespace iris;
        using namespace iris::gen;

        Agent        agents[5];
        const auto   familyUnit = FamilyUnit{0, 5};

        // Set up the agent family, first.
        for(auto i = 0; i < 5; i++)
        {
            agents[i].setUId(i);
        }

        for(auto j = 0; j < 5; j++)
        {
            // Now wire them up.
            wireFamilyUnit(agents[j], familyUnit);

            // And check the results.
            auto expected = Agent::Network(5);

            std::iota(expected.begin(), expected.end(), 0);
            expected.erase(std::find(expected.begin(), expected.end(), j));

            const auto result = agents[j].getNetwork();
            CHECK(expected == result);

            SECTION("Verify there are no duplicates.")
            {
                testForDuplicates(result);
            }

            SECTION("Verify there are no loops.")
            {
                testForLoops(j, result);
            }
        }
    }
}

TEST_CASE("Verify out-group connection creation is correct and unique.")
{
    // Create a small "sample set" of agents to use.
    // For this particular exercise, we are going to make a "small world";
    // that is, everyone will be connected to everyone else.
    //
    // This will allow us to verify that both the sample space search
    // (finally) works correctly as well as the algorithm's denial of
    // repeat selections.
    //
    // This is divided into two sections to test varying family sizes.

    using namespace iris;
    using namespace iris::gen;
    using namespace iris::types;
    using namespace std;

    random_device    seed;
    mersenne_twister random(seed());

    Agent*           agents = new Agent[20];

    for(auto i = 0; i < 20; i++)
    {
        agents[i].setFamilySize(1);
        agents[i].setUId(i);
    }
    
    SECTION("Verify out-group connections work for singular families only.")
    {        
        const uint32 outConnections = 19;
        
        for(auto j = 0; j < 20; j++)
        {
            wireOutGroup(j, agents, 20, outConnections, 1.0, 1.0, random);
        }

        SECTION("Verify that there are no duplicates or loops.")
        {
            for(auto k = 0; k < 20; k++)
            {
                auto network = agents[k].getNetwork();

                CHECK(network.size() == 19);
                
                testForDuplicates(network);
                testForLoops(k, network);
            }
        }
    }

    SECTION("Verify no out-group connects results in zero-size networks.")
    {
        const uint32 outConnections = (uint32)0;
        
        for(auto j = 0; j < 20; j++)
        {
            wireOutGroup(j, agents, 20, outConnections, 1.0, 1.0, random);
        }

        SECTION("Verify that there are no duplicates or loops.")
        {
            for(auto k = 0; k < 20; k++)
            {
                auto network = agents[k].getNetwork();

                CHECK(network.size() == 0);
                
                testForDuplicates(network);
                testForLoops(k, network);
            }
        }
    }

    SECTION("Verify that out connection amounts that result in potential"
            " networks larger than the total number of agents in the simulation"
            " are handled correctly.")
    {
        const uint32 outConnections = 1000000;

        for(auto j = 0; j < 20; j++)
        {
            wireOutGroup(j, agents, 20, outConnections, 1.0, 1.0, random);
        }

        SECTION("Verify that there are no duplicates or loops.")
        {
            for(auto k = 0; k < 20; k++)
            {
                auto network = agents[k].getNetwork();

                CHECK(network.size() == 19);
                
                testForDuplicates(network);
                testForLoops(k, network);
            }
        }
    }

    delete[] agents;
}
