#include <catch.hpp>

#include "iris/Agent.hpp"
#include "iris/Types.hpp"

TEST_CASE("Verify initialization properties.")
{
    SECTION("Verify default values.")
    {
        iris::Agent agent;

        CHECK(agent.getFamilySize() == 0);
        CHECK(agent.getPrivilege() == 0);
        CHECK(agent.isPowerful() == false);
        CHECK(agent.getUId() == 0);
    }
}

TEST_CASE("Verify network management methods work correctly.")
{
    using namespace iris;
    using namespace iris::types;

    Agent agent;

    // Sample attributes.
    agent.setFamilySize(2);
    agent.setUId(45);
    
    // Invent a sample network.
    agent.addConnection(3);
    agent.addConnection(5);
    agent.addConnection(6);
    
    SECTION("Verify that a full network is not detected under normal"
            " constraints.")
    {
        // In this case, normal constraints are where the number of potential
        // out-family connections does not potentially exceed the total number
        // of agents in a simulation.
        const auto outConnections = static_cast<uint32>(10);
        const auto totalAgents    = static_cast<AgentID>(200);

        CHECK(agent.isNetworkFull(outConnections, totalAgents) == false);
    }

    SECTION("Verify that a full network is detected under normal constraints.")
    {
        const auto outConnections = static_cast<uint32>(1);
        const auto totalAgents    = static_cast<AgentID>(200);

        CHECK(agent.isNetworkFull(outConnections, totalAgents) == true);
    }

    SECTION("Verify that a full network is not detected under abnormal"
            " constraints.")
    {
        const auto outConnections = static_cast<uint32>(100000);
        const auto totalAgents    = static_cast<AgentID>(200);

        CHECK(agent.isNetworkFull(outConnections, totalAgents) == false);
    }

    SECTION("Verify that a full network is detected under abnormal"
            " constraints.")
    {
        const auto outConnections = static_cast<uint32>(100000);
        const auto totalAgents    = static_cast<AgentID>(4);

        CHECK(agent.isNetworkFull(outConnections, totalAgents) == true);
    }

    SECTION("Verify that connection check works correctly.")
    {
        CHECK(agent.isConnectedTo(3) == true);
        CHECK(agent.isConnectedTo(5) == true);
        CHECK(agent.isConnectedTo(4) == false);
    }

    SECTION("Verify that adding a connection sorts correctly.")
    {
        const auto expected0 = Agent::Network{3, 4, 5, 6};
        const auto expected1 = Agent::Network{0, 3, 4, 5, 6};
        const auto expected2 = Agent::Network{0, 3, 4, 5, 6, 7};
        
        agent.addConnection(4);
        CHECK(agent.getNetwork() == expected0);

        agent.addConnection(0);
        CHECK(agent.getNetwork() == expected1);

        agent.addConnection(7);
        CHECK(agent.getNetwork() == expected2);
    }
}

TEST_CASE("Ensure removal of powerful agents works correctly.")
{
    using namespace iris;
    using namespace iris::types;
    
    Agent*           agents = new Agent[20];

    for(auto i = 0; i < 10; i++)
    {
        agents[i].setFamilySize(1);
        agents[i].setUId(i);
    }

    agents[3].setPowerful(true);
    agents[7].setPowerful(true);

    SECTION("Simple case where powerful agents are present (to be preserved).")
    {
        auto       network  = Agent::Network{2, 3, 4, 7, 9};
        const auto expected = Agent::Network{3, 7};

        agents[0].removeNonPowerful(network, agents, 10);
        CHECK(network == expected);
    }

    SECTION("Correctly handles the case where there are no powerful agents"
            " to preserve.")
    {
        auto       network  = Agent::Network{2, 4, 6, 9, 10};
        const auto expected = Agent::Network{};

        agents[0].removeNonPowerful(network, agents, 10);
        CHECK(network == expected);
    }

    SECTION("Correctly handles the case where there are only powerful agents"
            " to preserve.")
    { 
        auto       network  = Agent::Network{3, 7};
        const auto expected = Agent::Network{3, 7};

        agents[0].removeNonPowerful(network, agents, 10);
        CHECK(network == expected);       
    }

    SECTION("Correctly extracts the powerful agents.")
    {
        const auto network  = Agent::Network{1, 2, 3, 4};
        const auto expected = Agent::Network{3};

        const auto result   = agents[0].extractPowerful(network, agents, 10);
        CHECK(result == expected);
    }

    delete[] agents;
}

TEST_CASE("Verify that random selection of in-group works correctly.")
{
    using namespace iris;
    using namespace iris::types;
    using namespace std;

    random_device    seed;
    mersenne_twister random(seed());

    Agent*           agents = new Agent[20];

    for(auto i = 0; i < 10; i++)
    {
        agents[i].setFamilySize(1);
        agents[i].setUId(i);
    }

    agents[0].addConnection(3);
    agents[0].addConnection(4);
    agents[0].addConnection(6);
    agents[0].addConnection(8);

    SECTION("Selection of in-group works as expected.")
    {
        const auto expected = Agent::Network{3, 4, 6, 8};
        auto       result   = agents[0].obtainRandomInGroup(3, random);
        auto       count    = 0;
        
        for(Agent::Network::size_type i = 0; i < result.size(); i++)
        {
            if(std::find(expected.begin(), expected.end(), result[i])
                != expected.end())
            {
                count++;
            }
        }

        CHECK(count == 3);
        CHECK(result.size() == 3);
    }
    
    SECTION("Selection of in-group works as expected when search at capacity.")
    {
        const auto expected = Agent::Network{3, 4, 6, 8};
        auto       result   = agents[0].obtainRandomInGroup(4, random);

        std::sort(result.begin(), result.end());
        CHECK(result == expected);
    }

    SECTION("Selection of in-group works as expected even when the number of"
            " desired agents is greater than the number available in the"
            " network.")
    { 
        const auto expected = Agent::Network{3, 4, 6, 8};
        auto       result   = agents[0].obtainRandomInGroup(40, random);

        std::sort(result.begin(), result.end());
        CHECK(result == expected);       
    }

    SECTION("Selection of in-group returns nothing when qIn is zero.")
    {
        const auto expected = Agent::Network{};
        auto       result   = agents[0].obtainRandomInGroup(0, random);

        CHECK(result == expected);
    }
    
    delete[] agents;
}

TEST_CASE("Verify that random selection of out-group works correctly.")
{
    using namespace iris;
    using namespace iris::types;
    using namespace std;

    random_device    seed;
    mersenne_twister random(seed());

    Agent*           agents = new Agent[20];

    for(auto i = 0; i < 10; i++)
    {
        agents[i].setFamilySize(1);
        agents[i].setUId(i);
    }

    agents[0].addConnection(2);
    agents[0].addConnection(5);
    agents[0].addConnection(9);
    agents[0].addConnection(14);
    agents[0].addConnection(18);
    agents[0].addConnection(19);

    SECTION("Selection of out-group works as expected.")
    {
        const auto expected = Agent::Network{
            1, 3, 4, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17
        };
        auto       result   = agents[0].obtainRandomOutGroup(5, 20, random);
        auto       count    = 0;
        
        for(Agent::Network::size_type i = 0; i < result.size(); i++)
        {
            if(std::find(expected.begin(), expected.end(), result[i])
                != expected.end())
            {
                count++;
            }
        }

        CHECK(count == 5);
        CHECK(result.size() == 5);
    }
    
    SECTION("Verify that selection works as expected when requesting capacity.")
    {
        const auto expected = Agent::Network{
            1, 3, 4, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17
        };
        auto       result   = agents[0].obtainRandomOutGroup(13, 20, random);

        std::sort(result.begin(), result.end());
        CHECK(result == expected);
    }

    SECTION("Verify that selection works as expected when requesting"
            " over capacity (barely).")
    {
        const auto expected = Agent::Network{
            1, 3, 4, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17
        };
        auto       result   = agents[0].obtainRandomOutGroup(14, 20, random);

        std::sort(result.begin(), result.end());
        CHECK(result == expected);        
    }
    
    SECTION("Verify that selection works as expected when requesting"
            " over capacity.")
    {
        const auto expected = Agent::Network{
            1, 3, 4, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17
        };
        auto       result   = agents[0].obtainRandomOutGroup(133, 20, random);

        std::sort(result.begin(), result.end());
        CHECK(result == expected);        
    }

    SECTION("Out-group selection returns zero when qOut is also zero.")
    {
        const auto expected = Agent::Network{};
        const auto result   = agents[0].obtainRandomOutGroup(0, 20, random);

        CHECK(result == expected);
    }
    
    delete[] agents;
}

TEST_CASE("Verify that side computations work correctly.")
{
    using namespace iris;
    using namespace iris::types;
    using namespace std;

    random_device    seed;
    mersenne_twister random(seed());

    Agent*           agents = new Agent[20];

    for(auto i = 0; i < 10; i++)
    {        
        agents[i].setFamilySize(1);
        agents[i].setInitialBehavior(Uint32List{static_cast<uint32>(i)});
        agents[i].setUId(i);
    }

    SECTION("Simple computation test where a single agent does not"
            " share the same behavior.")
    {
        const auto network  = Agent::Network{3};
        const auto expected = Agent::Sides{1, 0};

        const auto result  = agents[0].computeSides(0, 0, network, agents,
                                                    10, 0);
        CHECK(result.first  == expected.first);
        CHECK(result.second == expected.second);
    }

    SECTION("Simple computation test where a single agent"
            " does share the same behavior.")
    { 
        const auto network  = Agent::Network{3};
        const auto expected = Agent::Sides{0, 1};

        const auto result  = agents[0].computeSides(0, 3, network, agents,
                                                    10, 0);
        CHECK(result.first  == expected.first);
        CHECK(result.second == expected.second);
    }

    SECTION("Verify that multiple agents with differing behaviors"
            " return the correct number of against.")
    {
        const auto network  = Agent::Network{1, 2, 3};
        const auto expected = Agent::Sides{3, 0};

        const auto result  = agents[0].computeSides(0, 6, network, agents,
                                                    10, 0);
        CHECK(result.first  == expected.first);
        CHECK(result.second == expected.second);
    }

    SECTION("Verify that multiple agents with differing behaviors"
            " return the correct number of against AND in favor.")
    {
        const auto network  = Agent::Network{1, 2, 3, 4};
        const auto expected = Agent::Sides{2, 2};

        agents[1].setInitialBehavior(Uint32List{static_cast<uint32>(2)});
        
        const auto result  = agents[0].computeSides(0, 2, network, agents,
                                                    10, 0);
        CHECK(result.first  == expected.first);
        CHECK(result.second == expected.second);
    }
    
    delete[] agents;
}

TEST_CASE("Verify that updating an agent's state works correctly.")
{
    using namespace iris;
    using namespace iris::types;

    Agent agent;

    agent.setInitialBehavior(Uint32List{static_cast<uint32>(0)});
    
    SECTION("Updating across two different time steps works.")
    {
        agent.updateState(0, 4, 0);
        agent.updateState(0, 54, 1);

        const auto expected0 = 4;
        const auto expected1 = 54;
        const auto expected2 = Uint32List{static_cast<uint32>(54)};

        CHECK(agent.getBehaviorAt(0, 0) == expected0);
        CHECK(agent.getBehaviorAt(0, 1) == expected1);
        CHECK(agent.getBehavior() == expected2);
    }

    SECTION("Test multi-variate update.")
    {
        const auto initialBehav = Uint32List{
            static_cast<uint32>(3),
            static_cast<uint32>(4),
            static_cast<uint32>(1)
        };
        agent.setInitialBehavior(initialBehav);
        
        agent.updateState(0,  4, 0);
        agent.updateState(2, 54, 1);
        agent.updateState(1, 23, 2);

        const auto expected00 = 4;
        const auto expected01 = 4;
        const auto expected02 = 54;

        const auto expected10 = 4;
        const auto expected11 = 23;
        const auto expected12 = 54;
        
        const auto expected2 = Uint32List{
            static_cast<uint32>(4),
            static_cast<uint32>(23),
            static_cast<uint32>(54)
        };

        CHECK(agent.getBehaviorAt(0, 1) == expected00);
        CHECK(agent.getBehaviorAt(1, 1) == expected01);
        CHECK(agent.getBehaviorAt(2, 1) == expected02);

        CHECK(agent.getBehaviorAt(0, 2) == expected10);
        CHECK(agent.getBehaviorAt(1, 2) == expected11);
        CHECK(agent.getBehaviorAt(2, 2) == expected12);

        CHECK(agent.getBehavior() == expected2);
        CHECK_THROWS(agent.getBehaviorAt(0, 0));
    }
}

TEST_CASE("Verify that computing the outcome directly (via sides) is correct.")
{
    using namespace iris;
    using namespace iris::types;

    Agent agent;

    SECTION("Simple case.")
    {
        const auto sides    = Agent::Sides{
            static_cast<uint32>(0),
            static_cast<uint32>(1),
        };
        const auto expected = Agent::Outcome::Keep;
        const auto result   = agent.computeOutcomeDirectly(sides);

        CHECK(result == expected);
    }

    SECTION("Tie always favors the agent (in favor).")
    {
        const auto sides    = Agent::Sides{
            static_cast<uint32>(32),
            static_cast<uint32>(32),
        };
        const auto expected = Agent::Outcome::Keep;
        const auto result   = agent.computeOutcomeDirectly(sides);

        CHECK(result == expected);
    }

    SECTION("Simple case where change is favored.")
    {
        const auto sides    = Agent::Sides{
            static_cast<uint32>(27),
            static_cast<uint32>(23),
        };
        const auto expected = Agent::Outcome::Change;
        const auto result   = agent.computeOutcomeDirectly(sides);

        CHECK(result == expected);        
    }
}

TEST_CASE("Ensure utility function is roughly accurate.")
{
    // All of the values in this section are compared to those produced by R.
    
    using namespace iris;
    using namespace iris::types;

    Agent agent;

    SECTION("Use base lambda.")
    {
        const auto lambda   = static_cast<fnumeric>(1);
        const auto x        = static_cast<uint32>(1);
        const auto expected = static_cast<fnumeric>(0.6321206);
        const auto result   = agent.computeUtility(lambda, x);

        CHECK(result == Approx(expected));
    }
}

TEST_CASE("Ensure that the communication type is determined correctly based upon"
          " the outcome of a social encounter.")
{
    using namespace iris;
    Agent agent;
    
    SECTION("Verify the two possible communication outcomes that are"
            " not \"neither\".")
    {
        const auto expected0 = Agent::CommType::Censored;
        const auto expected1 = Agent::CommType::Reinforced;
        
        const auto result0   =
            agent.determineCommType(2, 3, Agent::Outcome::Change);
        const auto result1   =
            agent.determineCommType(2, 2, Agent::Outcome::Keep);

        CHECK(result0 == expected0);
        CHECK(result1 == expected1);
    }

    SECTION("Verify the two possible communication outcomes that are"
            " \"neither\".")
    {
        const auto expected = Agent::CommType::Neither;

        const auto result0  =
            agent.determineCommType(2, 3, Agent::Outcome::Keep);
        const auto result1  =
            agent.determineCommType(2, 2, Agent::Outcome::Change);

        CHECK(result0 == expected);
        CHECK(result1 == expected);
    }
}
