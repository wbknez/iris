#include <catch.hpp>

#include <sstream>
#include <string>

#include "iris/Agent.hpp"
#include "iris/Types.hpp"

#include "iris/io/writer/AttributeWriter.hpp"

TEST_CASE("Ensure that agent attributes are written to a stream correctly.")
{
    using namespace iris;
    using namespace iris::io;
    using namespace iris::types;

    Agent* agents = new Agent[2];

    for(auto i = 0; i < 2; i++)
    {
        agents[i].setFamilySize(1);
        agents[i].setUId(i);

        const auto value    = static_cast<uint32>(i);
        const auto behavior = static_cast<uint32>(i);
        
        agents[i].setInitialValues(Uint32List{value});
        agents[i].setInitialBehavior(Uint32List{behavior});
    }

    agents[0].addConnection(1);
    agents[1].addConnection(0);

    SECTION("Simple test case.")
    {
        const auto expected = std::string(
            "AgentID,FamilySize,Power,Privilege,Values,Behavior\n"
            "0,1,0,0,0,0\n"
            "1,1,0,0,1,1\n"
        );
        auto       stream   = std::ostringstream();

        outputAttributes(stream, agents, 2);
        CHECK(stream.str() == expected);
    }

    SECTION("Additional test case with custom attributes.")
    {
        // As with the network writer, this example is not technically possible
        // in the program itself due to bounds enforcement.  Obviously, the
        // ability to write attributes correctly is all that matters here.
        agents[0].increasePrivilege();
        agents[0].setInitialBehavior(Uint32List{99, 12});
        agents[0].setUId(47);
        
        agents[1].setUId(100);
        agents[1].setInitialValues(Uint32List{45, 445});
        agents[1].setPowerful(true);

        const auto expected = std::string(
            "AgentID,FamilySize,Power,Privilege,Values,Behavior\n"
            "47,1,0,1,0,9912\n"
            "100,1,1,0,45445,1\n"
        );
        auto       stream   = std::ostringstream();

        outputAttributes(stream, agents, 2);
        CHECK(stream.str() == expected);
    }

    delete[] agents;
}
