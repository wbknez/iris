#include "iris/gen/PopulationDispenser.hpp"

#include <numeric>
#include <random>
#include <stdexcept>

namespace iris
{
    namespace gen
    {
        PopulationDispenser::PopulationDispenser()
        {}

        PopulationDispenser::~PopulationDispenser()
        {}
        
        void PopulationDispenser::checkPopulationRemainder(
            AgentID requiredPopulation)
        {
            using namespace iris::types;
            using namespace std;
            
            // Determine how many agents have been created thus far.
            const AgentID computedPopulation =
                accumulate(m_populations.begin(), m_populations.end(), 0);

            if(computedPopulation > requiredPopulation)
            {
                throw std::runtime_error("Computed population is greater"
                                         " than required!");
            }
            
            // Calculate the remainder.
            const AgentID remainder =
                requiredPopulation - computedPopulation;

            // Distribute the remainder to all groups in order (round-robin) if
            // necessary.
            if(remainder != 0)
            {
                AgentID currentIndex = 0;

                for(AgentID i = 0; i < remainder; i++)
                {
                    const AgentID currentGroupPop = m_populations[currentIndex];
                    m_populations[currentIndex] = currentGroupPop + 1;

                    currentIndex += 1;

                    if(currentIndex >= m_populations.size())
                    {
                        currentIndex = 0;
                    }
                }
            }
        }
        
        void PopulationDispenser::clear()
        {
            m_groups.clear();
            m_populations.clear();
        }
        
        bool PopulationDispenser::hasMore()
        {
            return !m_groups.empty();
        }
        
        void PopulationDispenser::initialize(
            const std::vector<types::fnumeric> &percentages,
            AgentID totalPopulation, bool requireAtLeastOne)
        {
            using namespace iris::types;
            using namespace std;

            typedef vector<fnumeric> PercentList;
            
            // Reset.
            this->clear();

            for(PercentList::size_type i = 0; i < percentages.size(); i++)
            {
                const fnumeric percent = percentages[i];

                // Obtain the total number of agents to dispense for this group.
                const auto groupPop =
                    (AgentID)floor(totalPopulation * percent);

                if(groupPop != 0)
                {
                    m_groups.push_back((uint32)i);
                    m_populations.push_back(groupPop);
                }
                else
                {
                    if(requireAtLeastOne)
                    {
                        m_groups.push_back((uint32)i);
                        m_populations.push_back(1);
                    }
                }
            }

            this->checkPopulationRemainder(totalPopulation);
        }
        
        types::uint32 PopulationDispenser::nextGroup(
            types::mersenne_twister &random)
        {
            using namespace iris::types;
            using namespace std;
            
            if(m_groups.size() == 0)
            {
                throw std::runtime_error("The dispenser is empty!");
            }
            
            typedef uniform_int_distribution<uint32> UintDistrib;
            UintDistrib distrib(0, m_groups.size() - 1);
            
            // Obtain a random group index to use.
            const uint32 groupSelection = distrib(random);

            // The indice of the group in question.
            const uint32 currentGroup = m_groups[groupSelection];
            const uint32 currentPopulation = m_populations[groupSelection] - 1;
            
            // Determine if the group's population needs have been exhausted.
            if(currentPopulation == 0)
            {
                m_groups.erase(m_groups.begin() + groupSelection);
                m_populations.erase(m_populations.begin() + groupSelection);
            }
            else
            {
                m_populations[groupSelection] = currentPopulation;
            }

            return currentGroup;
        }
    }
}
