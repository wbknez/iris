#include <catch.hpp>

#include <sstream>
#include <string>

#include "iris/Agent.hpp"
#include "iris/Types.hpp"

#include "iris/io/writer/NetworkWriter.hpp"

TEST_CASE("Ensure that agents are written to a stream correctly.")
{
    using namespace iris;
    using namespace iris::io;

    Agent* agents = new Agent[2];

    for(auto i = 0; i < 2; i++)
    {
        agents[i].setFamilySize(1);
        agents[i].setUId(i);
    }

    agents[0].addConnection(1);
    agents[1].addConnection(0);
    
    SECTION("Simple test case.")
    {
        const auto expected = std::string("From,To\n1,0\n0,1\n");
        auto       stream   = std::ostringstream();

        outputNetwork(stream, agents, 2);
        CHECK(stream.str() == expected);
    }

    SECTION("Additional test case with custom network.")
    {
        // Please note that this is not actually reflective of the program
        // itself, as this particular network structure is technically illegal
        // (agents '3' and '4' do not exist.).
        agents[0].addConnection(4);
        agents[1].addConnection(3);

        const auto expected = std::string(
            "From,To\n1,0\n4,0\n0,1\n3,1\n"
        );
        auto       stream   = std::ostringstream();

        outputNetwork(stream, agents, 2);
        CHECK(stream.str() == expected);
    }

    delete[] agents;
}
