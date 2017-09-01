#ifndef IRIS_POWER_WRITER_HPP_
#define IRIS_POWER_WRITER_HPP_

#include <sstream>
#include <string>

#include "iris/Types.hpp"

namespace iris
{
    class Agent;
    
    namespace io
    {
        /*!
         * Writes the graph of interactions between agents whose edges
         * represent power to the specified file.
         *
         * @param filename
         *        The name of the CSV file to write to.
         * @param agents
         *        The array of agents.
         * @param totalAgents
         *        The total number of agents present.
         */
        void writePower(const std::string& filename, Agent* const agents,
                        AgentID totalAgents);

        /*!
         * Writes the graph of interactions between agents whose edges
         * represent power to the specified stream.
         *
         * @param out
         *        The stream to write to.
         * @param agents
         *        The array of agents.
         * @param totalAgents
         *        The total number of agents present.
         */
        void outputPower(std::ostream& out, Agent* const agents,
                         AgentID totalAgents);
    }
}

#endif
