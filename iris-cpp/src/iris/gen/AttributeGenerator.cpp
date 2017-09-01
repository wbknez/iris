#include "iris/gen/AttributeGenerator.hpp"

#include <cmath>
#include <random>
#include <sstream>

#include "iris/Utils.hpp"

namespace iris
{
    namespace gen
    {
        types::uint32 convertListToInteger(const Uint32List& list)
        {
            using namespace iris::types;
            
            uint32 result      = 0;
            uint32 scaleFactor = list.size() - 1;

            for(Uint32List::size_type i = 0; i < list.size(); i++)
            {
                const auto exponent = scaleFactor - i;
                result += list[i] * (uint32)std::pow(10, exponent);
            }

            return result;
        }

        std::string convertListToString(const Uint32List& list)
        {
            using namespace iris;
            std::ostringstream stream;

            for(Uint32List::size_type i = 0; i < list.size(); i++)
            {
                stream << list[i];
            }

            return stream.str();
        }
        
        Uint32List createAttributeList(PopDispensers& dispensers,
                                      types::mersenne_twister& random)
        {
            Uint32List result(dispensers.size());
            
            for(Uint32List::size_type i = 0; i < dispensers.size(); i++)
            {
                const auto indice = dispensers[i].nextGroup(random);
                result[i] = indice;
            }

            return result;
        }

        PopDispensers createPopulationDispensers(Uint32List vars,
                                                 AgentID totalAgents)
        {
            typedef std::vector<types::fnumeric> Factors;
            
            PopDispensers result;
            
            // So, each value in the list represents the number of factors.  We
            // use equal probability for all factors, so the probability of each
            // is 1/f per indice.
            for(Uint32List::size_type i = 0; i < vars.size(); i++)
            {
                const auto currentFactor = vars[i];
                const types::fnumeric prob =
                    ((types::fnumeric)1) / currentFactor;
                PopulationDispenser disp;

                // The factored probabilities.
                Factors factors(currentFactor);

                // Fill up the factor probability.
                std::fill(factors.begin(), factors.end(), prob);

                // Set it up.
                disp.initialize(factors, totalAgents, true);

                // Done.
                result.push_back(disp);
            }

            return result;
        }

        Uint32List createSubAttributeList(const Uint32List& base,
                                          const Uint32List& target)
        {
            if(base.size() < target.size())
            {
                // This should be caught by the IO parser.
                throw std::runtime_error("Base size is smaller than target!");
            }

            return {base.begin(), base.begin() + target.size()};
        }
        
        void generateAttributes(Agent* const agents, AgentID totalAgents,
                                ValueList values, BehaviorList behaviors,
                                types::mersenne_twister& random)
        {         
            // Use two population vectors, one for the values and one for the
            // behaviors.
            PopDispensers valueDisp =
                createPopulationDispensers(values, totalAgents);

            // For each agent, create a new and unique set of values and
            // behaviors.
            for(AgentID i = 0; i < totalAgents; i++)
            {
                ValueList vList    = createAttributeList(valueDisp, random);
                BehaviorList bList = createSubAttributeList(vList, behaviors);
                
                agents[i].setInitialBehavior(bList);
                agents[i].setInitialValues(vList);
            }
        }

        void generatePowerfulAgents(Agent* const agents, AgentID totalAgents,
                                    types::fnumeric powerPercent,
                                    bool requireAtLeastOne,
                                    types::mersenne_twister& random)
        {
            using namespace iris::types;
            using namespace iris::util;
            
            typedef Agent::Network                         Network;
            typedef std::uniform_int_distribution<AgentID> UintDist;
            
            if(powerPercent == 0.0)
            {
                return;
            }

            // Compute the number of powerful agents to create.
            auto numPowerful =
                (AgentID)std::floor(totalAgents * powerPercent);

            // If the percentage is so low that it evaluates to zero, then see
            // what the caller wishes to do.
            if(numPowerful == 0 && requireAtLeastOne)
            {
                numPowerful = 1;
            }

            // Random selector.
            UintDist chooser(0, totalAgents - 1);
            // The already selected agents.
            Network  selected;
            
            for(AgentID i = 0; i < numPowerful; i++)
            {
                const auto chosen =
                    ensureRandom<AgentID>(chooser(random), selected,
                                          (AgentID)0, totalAgents);
                agents[chosen].setPowerful(true);
                util::sortedInsert(selected, chosen);
            }
        }

        PermuteList permuteList(const Uint32List& vars)
        {
            auto mutableList  = Uint32List(vars.size());
            auto permutes     = PermuteList();

            permuteList(vars, permutes, mutableList, 0);
            return permutes;
        }
        
        void permuteList(const Uint32List& vars, PermuteList& permutes,
                         Uint32List& mutableList, types::uint32 index)
        {
            for(types::uint32 i = 0; i < vars[index]; i++)
            {
                mutableList[index] = i;
                
                if(index >= (vars.size() - 1))
                {
                    const auto value = convertListToString(mutableList);
                    permutes.push_back(value);
                }
                else
                {
                    permuteList(vars, permutes, mutableList, index + 1);
                }
            }
        }
    }
}
