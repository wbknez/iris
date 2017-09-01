#include <catch.hpp>

#include <sstream>
#include <string>

#include "iris/Agent.hpp"
#include "iris/Types.hpp"

#include "iris/io/writer/StatisticsWriter.hpp"

TEST_CASE("Verify that the statistics writer outputs compiled histogram data"
          " correctly and in order.")
{
    using namespace iris;
    using namespace iris::io;
    using namespace iris::types;
    using namespace std;

    random_device    seed;
    mersenne_twister random(seed());

    Agent*           agents = new Agent[2];

    for(auto i = 0; i < 2; i++)
    {
        agents[i].setFamilySize(1);
        agents[i].setUId(i);

        const auto value    = static_cast<uint32>(i);
        const auto behavior = static_cast<uint32>(i);
        
        agents[i].setInitialValues(Uint32List{value});
        agents[i].setInitialBehavior(Uint32List{behavior});
    }

    StatisticsWriter statWriter;

    statWriter.initialize(Uint32List{2});
    
    SECTION("Verify that the header is written correctly.")
    {
        const auto expected = std::string("Time,Privilege,0,1\n");
        auto       stream   = ostringstream();

        statWriter.writeHeader(stream);
        CHECK(stream.str() == expected);
    }
    
    SECTION("Verify correct output for simple case.")
    {
        const auto expected = std::string(
            "0,0,1,1\n"
        );
        auto       stream   = ostringstream();

        statWriter.writeStatistics(stream, agents, 2, 0);
        CHECK(stream.str() == expected);
    }

    SECTION("Verify a slightly more complicated case over several time steps.")
    {
        // Once again, this is technically not correct but that's okay.
        
        const auto expected = std::string(
            "0,0,1,1\n"
            "1,1,1,1\n"
            "2,3,0,2\n"
            "3,4,2,0\n"
            "4,4,1,1\n"
        );
        auto       stream   = std::stringstream();

        // Time step: 0
        statWriter.writeStatistics(stream, agents, 2, 0);
        
        // Time step: 1
        agents[0].increasePrivilege();
        statWriter.writeStatistics(stream, agents, 2, 1);

        // Time step: 2
        agents[0].increasePrivilege();
        agents[0].setInitialBehavior(Uint32List{1});
        agents[1].increasePrivilege();

        statWriter.writeStatistics(stream, agents, 2, 2);
        
        // Time step: 3
        agents[0].setInitialBehavior(Uint32List{0});
        agents[1].setInitialBehavior(Uint32List{0});
        agents[1].increasePrivilege();

        statWriter.writeStatistics(stream, agents, 2, 3);
        
        // Time step: 4
        agents[1].setInitialBehavior(Uint32List{1});

        statWriter.writeStatistics(stream, agents, 2, 4);
        CHECK(stream.str() == expected);
    }

    SECTION("Verify a a complicated case with over a thousand privilege and"
            " over several time steps.")
    {
        // Once again, this is technically not correct but that's okay.
        
        const auto expected = std::string(
            "0,0,1,1\n"
            "1,1,1,1\n"
            "2,3,0,2\n"
            "3,4,2,0\n"
            "4,1004,1,1\n"
        );
        auto       stream   = std::stringstream();

        // Time step: 0
        statWriter.writeStatistics(stream, agents, 2, 0);
        
        // Time step: 1
        agents[0].increasePrivilege();
        statWriter.writeStatistics(stream, agents, 2, 1);

        // Time step: 2
        agents[0].increasePrivilege();
        agents[0].setInitialBehavior(Uint32List{1});
        agents[1].increasePrivilege();

        statWriter.writeStatistics(stream, agents, 2, 2);
        
        // Time step: 3
        agents[0].setInitialBehavior(Uint32List{0});
        agents[1].setInitialBehavior(Uint32List{0});
        agents[1].increasePrivilege();

        statWriter.writeStatistics(stream, agents, 2, 3);
        
        // Time step: 4
        agents[1].setInitialBehavior(Uint32List{1});

        for(int i = 0; i < 1000; i++)
        {
            agents[1].increasePrivilege();
        }
        
        statWriter.writeStatistics(stream, agents, 2, 4);
        CHECK(stream.str() == expected);
    }

    delete[] agents;
}
