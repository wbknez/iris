#ifndef IRIS_THREADING_HPP_
#define IRIS_THREADING_HPP_

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include "iris/Parameters.hpp"

namespace iris
{
    class Agent;
  
    class ThreadWorker
    {
        public:
            ThreadWorker(std::atomic<types::uint32>& complete,
                         std::atomic<types::uint64>& time);
            ThreadWorker(const ThreadWorker& worker);
            ~ThreadWorker();

            void initialize(Agent* agents, AgentID start, AgentID end,
                            Parameters params, BehaviorList behaviors);

            void join();
            void signal()
            { m_signal = true;  }

            void start(types::uint32 id);
            void stop();
        public:
            // These functions are for unit testing only !!!
      
            BehaviorList getBehaviorList() const;
            std::vector<AgentID> getIndices() const;
            Parameters getParameters() const;
            types::uint64 getTime() const;
      
        private:
            void prepareRandom();
            void run();

        private:
            Agent*                      m_agents;
            BehaviorList                m_behaviors;
            std::atomic<types::uint32>& m_complete;
            types::uint32               m_id;
            std::vector<AgentID>        m_indices;
            Parameters                  m_params;
            types::mersenne_twister     m_random;
            std::atomic<bool>           m_running;
            std::atomic<bool>           m_signal;
            std::thread                 m_thread;
            std::atomic<types::uint64>& m_time;
    };

    typedef std::vector<ThreadWorker> WorkerPool;

    class ThreadController
    {
        public:
            ThreadController();
            ~ThreadController();

            void initialize(Agent* agents, AgentID totalAgents,
                            const Parameters& params, BehaviorList behaviors,
                            types::uint32 numThreads,
                            std::atomic<types::uint64>& time);
      
            void signalAll();
            void start();
            void stopAll();
            void tearDown();
            void waitForCompletion();

        private:
            std::atomic<types::uint32> m_completions;
            WorkerPool                 m_pool;
    };
}

#endif
