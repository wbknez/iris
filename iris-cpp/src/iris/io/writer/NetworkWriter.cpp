#include "iris/io/writer/NetworkWriter.hpp"

#include <fstream>
#include <stdexcept>

#include "iris/Agent.hpp"

namespace iris
{
    namespace io
    {
        void writeNetwork(const std::string& filename,
                          iris::Agent* const agents,
                          AgentID totalAgents)
        {
            std::ofstream outfile(filename);

            if(!outfile.is_open())
            {
                throw std::runtime_error("Could not write to CSV file: " +
                                         filename);
            }

            outputNetwork(outfile, agents, totalAgents);
            outfile.close();
        }

        void outputNetwork(std::ostream& out,
                           iris::Agent* const agents,
                           AgentID totalAgents)
        {
            // Write a header.
            out << "From,To" << std:: endl;
            
            for(AgentID i = 0; i < totalAgents; i++)
            {
                const auto uid     = agents[i].getUId();
                const auto network = agents[i].getNetwork();

                for(Agent::Network::size_type j = 0; j < network.size(); j++)
                {
                    // This is an input-oriented graph, so the edges from all
                    // the agents in the network point *towards* the current
                    // agent, not away.
                    out << network[j] << "," << uid << std::endl;
                }
            }
        }
    }
}
