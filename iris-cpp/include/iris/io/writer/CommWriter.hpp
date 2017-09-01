#ifndef IRIS_COMM_WRITER_HPP_
#define IRIS_COMM_WRITER_HPP_

#include <sstream>
#include <string>

#include "iris/Types.hpp"

namespace iris
{
    class Agent;
    
    namespace io
    {
        /*!
         * Writes the communication graph that comprises the history of each
         * agent's interaction(s) with each other up to the current time step
         * to the specified file.
         *
         * @param filename
         *        The name of the CSV file to write to.
         * @param agents
         *        The array of agents.
         * @param totalAgents
         *        The total number of agents present.
         * @param time
         *        The current time step.
         */
        void writeComm(const std::string& filename, Agent* const agents,
                       AgentID totalAgents, types::uint64 time);

        /*!
         * Writes the communication graph that comprises the history of each
         * agent's interaction(s) with each other up to the current time step
         * to the specified stream.
         *
         * @param out
         *        The stream to write to.
         * @param agents
         *        The array of agents.
         * @param totalAgents
         *        The total number of agents present.
         * @param time
         *        The current time step.
         */
        void outputComm(std::ostream& out, Agent* const agents,
                        AgentID totalAgents, types::uint64 time);
    }
}

#endif
