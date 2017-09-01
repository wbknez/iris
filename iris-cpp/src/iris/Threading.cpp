#include "iris/Threading.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <numeric>
#include <stdexcept>

#include "iris/Agent.hpp"
#include "iris/Utils.hpp"

namespace iris
{
    ThreadWorker::ThreadWorker(std::atomic<types::uint32>& complete,
                               std::atomic<types::uint64>& time)
        : m_complete(complete), m_time(time)
    {
        m_running = false;
        m_signal  = false;
    }

    ThreadWorker::ThreadWorker(const ThreadWorker& worker)
        : m_agents(worker.m_agents), m_complete(worker.m_complete),
          m_indices(worker.m_indices), m_time(worker.m_time)
    {
        m_running = false;
        m_signal  = false;
    }

    ThreadWorker::~ThreadWorker()
    {
        if(m_thread.joinable())
        {
            m_running = false;
            m_thread.join();
        }
    }

    BehaviorList ThreadWorker::getBehaviorList() const
    {
        return m_behaviors;
    }

    std::vector<AgentID> ThreadWorker::getIndices() const
    {
        return m_indices;
    }

    Parameters ThreadWorker::getParameters() const
    {
        return m_params;
    }

    types::uint64 ThreadWorker::getTime() const
    {
        return m_time;
    }

    void ThreadWorker::initialize(Agent* agents, AgentID start,
                                  AgentID end, Parameters params,
                                  BehaviorList behaviors)
    {
        if(start >= end)
        {
            throw std::runtime_error("Interval is ill-formed - the end"
                                     " is greater than or equal to the start.");
        }
        
        m_agents    = agents;
        m_behaviors = behaviors;
        m_params    = params;
        
        m_indices.resize(end - start);
        std::iota(m_indices.begin(), m_indices.end(), start);
    }

    void ThreadWorker::join()
    {
        if(m_thread.joinable())
        {
            m_thread.join();
        }
    }

    void ThreadWorker::prepareRandom()
    {
        const auto time = std::chrono::high_resolution_clock::now()
               .time_since_epoch().count();
        const auto seed =
            std::abs(((time * 181) * ((m_id * 83) * 359)) % 104729);

        // Re-initialize.
        m_random = types::mersenne_twister(seed);
    }
    
    void ThreadWorker::run()
    {
        this->prepareRandom();
        
        while(m_running)
        {            
            // For a signal before doing anything.
            // *However*, also make the wait dependent on running state.
            util::spin(m_signal, true, m_running, true,
                       std::chrono::nanoseconds(1000));

            // Double check that we should be running.
            if(!m_running) { break; }

            // Copy the time for faster access.
            const types::uint64 timeCopy = m_time;
            
            // Rampage through the agent array and update everything.
            for(auto& indice : m_indices)
            {
                m_agents[indice].step(m_params, m_agents, m_params.m_n,
                                      m_behaviors, timeCopy, m_random);
            }
            
            // Reset cycle and begin waiting once more.
            m_complete += 1;
            m_signal    = false;
        }

        // Soft cleanup.
        m_signal = false;
    }
    
    void ThreadWorker::start(types::uint32 id)
    {
        if(m_running)
        {
            throw std::runtime_error("Thread has already been started!");
        }

        // Save the id.
        m_id = id;

        // Start me up!
        m_running = true;
        m_signal = false;
        
        m_thread = std::thread([this]() {
            this->run();
        });
    }

    void ThreadWorker::stop()
    {
        m_running = false;
    }

    ThreadController::ThreadController()
    {}

    ThreadController::~ThreadController()
    {
        // Use the ThreadWorker destructor to force threads to close.
        m_pool.clear();
    }

    void ThreadController::initialize(iris::Agent *agents,
                                      AgentID totalAgents,
                                      const Parameters &params,
                                      BehaviorList behaviors,
                                      types::uint32 numThreads,
                                      std::atomic<types::uint64>& time)
    {        
        if(m_pool.size() > 0)
        {
            throw std::runtime_error("Worker pool has already been"
                                     " initialized!");
        }
        
        const auto spread =
            static_cast<types::uint32>(std::floor(totalAgents / numThreads));
        
        for(types::uint32 i = 0; i < numThreads; i++)
        {
            const auto lowerBound = static_cast<AgentID>(spread * i);
            auto       upperBound = static_cast<AgentID>(spread * (i + 1));
            
            if(i >= (numThreads - 1))
            {
                upperBound = totalAgents;
            }
            
            auto worker = ThreadWorker(m_completions, time);

            worker.initialize(agents, lowerBound, upperBound,
                              params, behaviors);
            m_pool.push_back(worker);
        }
    }

    void ThreadController::signalAll()
    {
        for(auto& worker : m_pool)
        {
            worker.signal();
        }
    }

    void ThreadController::start()
    {
        for(WorkerPool::size_type i = 0; i < m_pool.size(); i++)
        {
            const auto id = static_cast<types::uint32>(i + 1);
            m_pool[i].start(id);
        }
    }

    void ThreadController::stopAll()
    {
        for(auto& worker : m_pool)
        {
            worker.stop();
        }
    }
    
    void ThreadController::tearDown()
    {
        m_pool.clear();
    }
    
    void ThreadController::waitForCompletion()
    {
        // Ensure that the completion rate is reset *before* waiting,
        // otherwise for small networks the threads might complete
        // in the time between the signal and actual wait.
        //
        // Obvious, really, but still.
        m_completions = 0;

        // Do a quick check to ensure the threads in this pool are
        // waiting correctly.
        if(m_completions != 0)
        {
            throw std::runtime_error("Completion number was reset before a wait"
                                     " - some of the threads are not waiting"
                                     " correctly!");
        }

        util::spin(m_completions, static_cast<types::uint32>(m_pool.size()),
                   std::chrono::nanoseconds(1000));
    }
}
