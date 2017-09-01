#include "iris/io/writer/PowerWriter.hpp"

#include <fstream>
#include <stdexcept>

#include "iris/Agent.hpp"

namespace iris
{
    namespace io
    {
        void writePower(const std::string& filename, Agent* const agents,
                        AgentID totalAgents)
        {
            std::ofstream outfile(filename);

            if(!outfile.is_open())
            {
                throw std::runtime_error("Could not write to CSV file: " +
                                         filename);
            }

            outputPower(outfile, agents, totalAgents);
            outfile.close();
        }
        
        void outputPower(std::ostream& out, Agent* const agents,
                         AgentID totalAgents)
        {
            using namespace iris::types;
            out << "From,To,Power" << std::endl;
            
            for(AgentID i = 0; i < totalAgents; i++)
            {
                const auto id       = agents[i].getUId();              
                for(const auto comm : agents[i].getInteractions())
                {
                    const auto     otherId      = comm.first;
                    const auto     communicated =
                        static_cast<fnumeric>(comm.second.m_communicated);
                    
                    const fnumeric power   =
                        comm.second.m_censored + comm.second.m_reinforced;
                    const fnumeric prob    = power / communicated;

                    out << id << "," << otherId << "," << prob << std::endl;
                }
            }
        }
    }
}
