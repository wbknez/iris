#include "iris/io/writer/CommWriter.hpp"

#include <fstream>
#include <stdexcept>

#include "iris/Agent.hpp"

namespace iris
{
    namespace io
    {
        void writeComm(const std::string& filename, Agent* const agents,
                       AgentID totalAgents, types::uint64 time)
        {
            std::ofstream outfile(filename);

            if(!outfile.is_open())
            {
                throw std::runtime_error("Could not write to CSV file: " +
                                         filename);
            }

            outputComm(outfile, agents, totalAgents, time);
            outfile.close();
        }
        
        void outputComm(std::ostream& out, Agent* const agents,
                         AgentID totalAgents, types::uint64 time)
        {
            using namespace iris::types;
            out << "From,To,Power" << std::endl;

            const auto realTime = static_cast<fnumeric>(2 * time);
            for(AgentID i = 0; i < totalAgents; i++)
            {
                const auto id       = agents[i].getUId();                
                for(const auto comm : agents[i].getInteractions())
                {
                    const auto     otherId = comm.first;
                    const auto     prob    =
                        comm.second.m_communicated / realTime;
                    
                    out << id << "," << otherId << "," << prob << std::endl;
                }
            }
        }
    }
}
