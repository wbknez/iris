#include "iris/gen/GraphGenerator.hpp"

#include <algorithm>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>

#include "iris/Utils.hpp"

namespace iris
{
    namespace gen
    {        
        types::uint32 chooseFamilySize(const types::fnumeric& p,
                                       const CDF& cdf)
        {
            for(CDF::size_type i = 0; i < cdf.size(); i++)
            {
                if(p <= cdf[i])
                {
                    return i;
                }
            }

            // This should never happen!
            throw std::runtime_error("p = " + util::toString(p) +
                                     ": The probability for the CDF is out"
                                     " of range!");
        }
        
        std::vector<types::fnumeric> createCDF(const io::CensusData& census)
        {
            CDF cdf(census.size());

            for(io::CensusData::size_type i = 0; i < census.size(); i++)
            {
                if(i == 0)
                {
                    cdf[0] = census[0];
                }
                else
                {
                    cdf[i] = cdf[i - 1] + census[i];
                }
            }

            // Fix the last element.
            cdf[cdf.size() - 1] = (types::fnumeric)1.0;
            return cdf;
        }
      
        void wireFamilyUnit(Agent& agent, const FamilyUnit& unit)
        {
            const auto id = agent.getUId();
            
            for(auto i = unit.first; i < unit.second; i++)
            {
                if(i == id)
                {
                    continue;
                }

                agent.addConnection(i);
            }
        }
        
        void wireGraph(Agent* const agents, AgentID totalAgents,
                       const io::CensusData census,
                       types::uint32 outConnections,
                       types::fnumeric connectionProb,
                       types::fnumeric recipProb,
                       types::mersenne_twister& random)
        {
            using namespace iris;
            using namespace iris::types;
            
            typedef std::uniform_real_distribution<fnumeric> FDist;

            CDF        cdf = createCDF(census);
            FDist      chooser(0.0, 1.0);
            AgentID    counter = 0;
            FamilyUnit unit;

            while(counter < totalAgents)
            {
                // Choose a new family unit.
                //
                // Add 1 to offset base 0.
                const fnumeric p = chooser(random);
                const uint32 familySize = chooseFamilySize(p, cdf) + 1;

                // Create a "new" family unit.
                unit.first  = counter;
                unit.second = counter + familySize;

                // Check for overflow.
                if(unit.second > totalAgents)
                {
                    unit.second = totalAgents;
                }

                for(auto i = unit.first; i < unit.second; i++)
                {
                    wireFamilyUnit(agents[i], unit);
                    agents[i].setFamilySize(familySize);
                    wireOutGroup(i, agents, totalAgents, outConnections,
                                 connectionProb, recipProb, random);              
                }
                
                // Finally, update the counter itself.
                counter += familySize;
            }
        }

        void wireOutGroup(const AgentID& id, Agent* const agents,
                          AgentID totalAgents, types::uint32 outConnections,
                          types::fnumeric connectionProb,
                          types::fnumeric recipProb,
                          types::mersenne_twister& random)
        {
            using namespace iris::types;
            using namespace iris::util;
            using namespace std;
            
            // If the number of out-going connections is zero,
            // then do not bother.
            if(outConnections == 0)
            {
                return;
            }
            
            typedef uniform_real_distribution<fnumeric> FDist;
            typedef uniform_int_distribution<AgentID>   UintDist;      
            
            // Obtain the current network.
            auto network = agents[id].getNetwork();
            
            // The random distributions to use.
            FDist    fdist(0.0, 1.0);
            UintDist udist(0, totalAgents - 1);

            // The actual number of maximum connections to make.
            const auto    familySize = agents[id].getFamilyConnections();

            // Figure out what the upper bound is, here.
            const AgentID upperBound =
                ((outConnections + familySize) > (totalAgents - 1)) ?
                (totalAgents - 1) : (outConnections + familySize);
            const AgentID maxConnections = upperBound - network.size();

#ifdef IRIS_VISUAL_DEBUG
            using namespace iris::util::term;

            Sequence cyan(Color::Cyan);
            Sequence def(Color::Default);
            
            std::cout << cyan << "*" << def << " ID: " << id << std::endl;
            std::cout << "Family size: " << familySize << std::endl;
            std::cout << "Max connections: " << maxConnections << std::endl;
#endif
            
            // Ensure the current ID is also checked.
            sortedInsert(network, id);
            
            for(AgentID i = 0; i < maxConnections; i++)
            {
                // Determine if a new connection should be made.
                const auto shouldConnect = fdist(random);

                if(shouldConnect > connectionProb)
                {
                    continue;
                }

                // The new agent to connect to.
                auto newIndex = udist(random);

#ifdef IRIS_WARN_ON_NONUNIQUE_RANDOM
                try
                {
#endif
                
                // Ensure it is uniquely random.
                newIndex =
                    ensureRandom<AgentID>(newIndex, network,
                                          (AgentID)0, totalAgents);

#ifdef IRIS_VISUAL_DEBUG
                if(find(network.begin(), network.end(), newIndex)
                    != network.end())
                {
                    std::cout << "Inserting a duplicate! " << " with"
                              << newIndex << std::endl;
                }

                if(agents[id].alreadyConnectedTo(newIndex))
                {
                    std::cout << "Agent detects a duplicate too! " << newIndex << std::endl;
                }
#endif
                
                // Place into the network (again) in sorted order.
                sortedInsert(network, newIndex);

                // Connect to the agent.
                agents[id].addConnection(newIndex);

#ifdef IRIS_VISUAL_DEBUG
                std::cout << "Added: " << id << " " << newIndex << std::endl;
#endif
                
                // Determine reciprocity.
                const auto shouldRecip = fdist(random);

                const auto makeRecip = shouldRecip <= recipProb;
                const auto isFull =
                    agents[newIndex].isNetworkFull(outConnections, totalAgents);
                const auto isConnected = agents[newIndex].isConnectedTo(id);
                
                if(makeRecip && !isFull && !isConnected)
                {
                    agents[newIndex].addConnection(id);

#ifdef IRIS_VISUAL_DEBUG
                    std::cout << "Recip: " << newIndex << " " << id << std::endl;
#endif
                }

#ifdef IRIS_WARN_ON_NONUNIQUE_RANDOM
            }
            catch(std::runtime_error& re)
            {
                using namespace iris::util::term;

                Sequence yellow(Color::Yellow);
                Sequence def(Color::Default);

                std::cerr << yellow << "*" << def << " Non-unique random"
                          << " number ignored." << std::endl;
            }
#endif
            }

#ifdef IRIS_VISUAL_DEBUG
            std::cout << std::endl;
#endif
        }
    }
}
