#include "iris/io/writer/AttributeWriter.hpp"

#include <fstream>

#include "iris/Agent.hpp"

#include "iris/gen/AttributeGenerator.hpp"

namespace iris
{
    namespace io
    {
        void writeAttributes(const std::string& filename,
                             Agent* const agents,
                             AgentID totalAgents)
        {
            std::ofstream outfile(filename);

            if(!outfile.is_open())
            {
                throw std::runtime_error("Could not write to CSV file: " +
                                         filename);
            }

            outputAttributes(outfile, agents, totalAgents);
            outfile.close();
        }
        
        void outputAttributes(std::ostream& out, Agent* const agents,
                              AgentID totalAgents)
        {
            using namespace iris;
            using namespace gen;

            // Write header.
            out << "AgentID,FamilySize,Power,Privilege,"
                << "Values,Behavior" << std::endl;
            
            for(AgentID i = 0; i < totalAgents; i++)
            {
                // Grab relevant information as locals for simplicity.
                const auto id         = agents[i].getUId();
                const auto familySize = agents[i].getFamilySize();
                const auto values     = agents[i].getValues();
                const auto behavior   = agents[i].getBehavior();
                const auto privilege  = agents[i].getPrivilege();
                const auto power      = agents[i].isPowerful();

                out << id << "," << familySize << "," << power << ","
                    << privilege << "," << convertListToString(values)
                    << "," << convertListToString(behavior) << std::endl;
            }
        }
    }
}
