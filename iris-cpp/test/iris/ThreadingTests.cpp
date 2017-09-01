#include <catch.hpp>

#include <algorithm>
#include <atomic>
#include <thread>
#include <vector>

#include "iris/Agent.hpp"
#include "iris/Parameters.hpp"
#include "iris/Types.hpp"
#include "iris/Threading.hpp"
#include "iris/Utils.hpp"

typedef std::vector<iris::AgentID> IdVector;

TEST_CASE("Verify that std::iota works as expected.")
{
    SECTION("Test simple range [a, b).")
    {
        const auto a = 5;
        const auto b = 15;

        auto       vec      = IdVector{};
        auto       expected = IdVector{5, 6, 7, 8, 9, 10, 11, 12, 13, 14};

        vec.resize(b - a);
        CHECK(vec.capacity() == 10);
        
        std::iota(vec.begin(), vec.end(), a);
        CHECK(vec == expected);
    }
}

TEST_CASE("Verify spread algorithm works correctly.")
{
    SECTION("Test incorrect range causes an exception.")
    {
        using namespace iris;
        using namespace iris::types;
        using namespace std;

        const auto           behavior   = Uint32List{};
        std::atomic<uint32>  completion = {0};
        auto                 params     = Parameters();
        std::atomic<uint64>  time       = {0};
        auto                 worker     = ThreadWorker(completion, time);

        CHECK_THROWS(worker.initialize(NULL, 3, 2, params, behavior));
        CHECK_THROWS(worker.initialize(NULL, 4, 4, params, behavior));
    }
    
    SECTION("Test that range is calculated correctly.")
    {
        using namespace iris;
        using namespace iris::types;
        
        const auto totalAgents = 100;
        const auto numThreads  = 3;
        
        auto       result      = IdVector{};
        const auto expected    = IdVector{0, 33, 33, 66, 66, 100};
        const auto spread      = static_cast<uint32>(totalAgents / numThreads);

        CHECK(spread == 33);
        for(uint32 i = 0; i < numThreads; i++)
        {
            const auto lowerBound = static_cast<AgentID>(spread * i);
            auto       upperBound = static_cast<AgentID>(spread * (i + 1));

            if(i >= (numThreads - 1))
            {
                upperBound = totalAgents;
            }

            result.push_back(lowerBound);
            result.push_back(upperBound);
        }

        CHECK(result == expected);
    }
}

TEST_CASE("Verify that the signal framework works correctly.")
{
    SECTION("Check that the atomic-reference idiom works as expected.")
    {
        using namespace iris::types;
        using namespace iris::util;
        using namespace std;

        atomic<uint32> counter   = {0};
        atomic<bool>   running   = {true};
        atomic<bool>   signal    = {false};

        const auto     duration  = std::chrono::nanoseconds(1000);
        const auto     lambda    = 
            [&counter, duration, &running, &signal](){
            while(running)
            {
                spin(signal, true, duration);
                counter = counter + 1;
                signal  = false;
            }
        };
        thread         worker    = thread(lambda);

        // First round.
        CHECK(counter == 0);
        CHECK(running == true);
        CHECK(signal == false);

        // Fire.
        signal = true;
        spin(signal, false, duration);

        // Second round. 
        CHECK(counter == 1);
        CHECK(running == true);
        CHECK(signal == false);       

        // Fire
        signal = true;
        spin(signal, false, duration);

        CHECK(counter == 2);
        CHECK(running == true);
        CHECK(signal == false);

        // Main library does not have this problem.
        running = false;
        signal = true;
        
        // Clean up.
        worker.join();
    }
}

TEST_CASE("Verify that ThreadWorkers store and access external values"
          " correctly.")
{
    using namespace iris;
    using namespace iris::types;
    using namespace std;

    atomic<uint32> complete;
    atomic<uint64> time     = {1};
    
    Agent*       agents = new Agent[3]; 
    ThreadWorker worker(complete, time);

    BehaviorList behavList  = {2, 3, 2};
    Parameters   params;

    // Set up the parameters.
    params.m_lambda         = 0.12;
    params.m_n              = 10000;
    params.m_outConnections = 15;
    params.m_powerPercent   = 0;
    params.m_qIn            = 3;
    params.m_qOut           = 2;
    params.m_resist         = 0.5;
    params.m_resistMax      = 0.95;
    params.m_resistMin      = 0.05;
    params.m_steps          = 2000;
    params.m_prob           = 0.8;
    params.m_recip          = 1.0;
    
    SECTION("Ensure behaviors and parameters are copied correctly.")
    {
        worker.initialize(agents, static_cast<AgentID>(0),
                          static_cast<AgentID>(3),params,
                          behavList);

        const auto resultBehav  = worker.getBehaviorList();
        const auto resultParams = worker.getParameters();

        CHECK(resultBehav == behavList);
        
        CHECK(resultParams.m_lambda == params.m_lambda);
        CHECK(resultParams.m_n == params.m_n);
        CHECK(resultParams.m_outConnections == params.m_outConnections);
        CHECK(resultParams.m_powerPercent == Approx(params.m_powerPercent));
        CHECK(resultParams.m_qIn == params.m_qIn);
        CHECK(resultParams.m_qOut == params.m_qOut);
        CHECK(resultParams.m_resist == Approx(params.m_resist));
        CHECK(resultParams.m_resistMax == Approx(params.m_resistMax));
        CHECK(resultParams.m_resistMin == Approx(params.m_resistMin));
        CHECK(resultParams.m_steps == params.m_steps);
        CHECK(resultParams.m_prob == Approx(params.m_prob));
        CHECK(resultParams.m_recip == Approx(params.m_recip));
    }

    delete[] agents;
}
