/*!
 *
 */
#ifndef IRIS_NETWORK_WRITER_HPP_
#define IRIS_NETWORK_WRITER_HPP_

#include <ostream>
#include <string>

#include "iris/Types.hpp"

namespace iris
{
    class Agent;
    
    namespace io
    {

        /*!
         * Writes the social network created from the specified collection of
         * agents to a CSV (comma separated value) file.
         *
         * @param filename
         *        The name of the CSV file to write to.
         * @param agents
         *        The array of agents.
         * @param totalAgents
         *        The total number of agents present.
         */
        void writeNetwork(const std::string& filename,
                          iris::Agent* const agents,
                          AgentID totalAgents);
        
        /*!
         * Writes the social network represented by the specified collection of
         * agents to a stream in CSV (comma separated value) form.
         *
         * @param out
         *        The stream to write to.
         * @param agents
         *        The array of agents.
         * @param totalAgents
         *        The total number of agents present.
         */
        void outputNetwork(std::ostream& out, iris::Agent* const agents,
                           AgentID totalAgents);
    }
}

#endif
