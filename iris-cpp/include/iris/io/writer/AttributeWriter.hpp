/*!
 *
 */
#ifndef IRIS_ATTRIBUTE_WRITER_HPP_
#define IRIS_ATTRIBUTE_WRITER_HPP_

#include <sstream>
#include <string>

#include "iris/Types.hpp"

namespace iris
{
    class Agent;
    
    namespace io
    {
        /*!
         * Writes the attributes of all agents to the specified file, one
         * agent per line.
         *
         * When using iGraph in R, the output from this function is used to
         * supply vertex information.
         *
         * @param filename
         *        The name of the CSV file to write to.
         * @param agents
         *        The array of agents.
         * @param totalAgents
         *        The total number of agents present.
         */
        void writeAttributes(const std::string& filename,
                             Agent* const agents,
                             AgentID totalAgents);

        /*!
         * Writes the attributes of all agents to the specified stream, one
         * agent per line.
         *
         * @param out
         *        The stream to write to.
         * @param agents
         *        The array of agents.
         * @param totalAgents
         *        The total number of agents present.
         */
        void outputAttributes(std::ostream& out, Agent* const agents,
                              AgentID totalAgents);
    }
}

#endif
