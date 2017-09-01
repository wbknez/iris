/*!
 * Contains mechanisms and structures to create random populations with specific
 * composition requirements.
 */
#ifndef IRIS_POPULATION_DISPENSER_HPP_
#define IRIS_POPULATION_DISPENSER_HPP_

#include <cmath>
#include <vector>

#include "iris/Types.hpp"

namespace iris
{
    namespace gen
    {
        /*!
         * Represents a mechanism for distributing a finite, pre-computed
         * collection of populations over a simulation space in a numerically
         * static manner.
         */
        class PopulationDispenser
        {
            public:
                /*! Constructor. */
                PopulationDispenser();
                
                /*! Destructor. */
                ~PopulationDispenser();

                /*!
                 * Clears the available groups and population counters in
                 * advance of being reused.
                 */
                void clear();

                /*!
                 * Returns whether or not there are any groups with available
                 * populations left to be created or used.
                 *
                 * @return Whether any group populations remain.
                 */                
                bool hasMore();

                /*!
                 * Initializes this population dispenser by computing per-group
                 * populations according to the specified population percentage
                 * distribution.
                 *
                 * @param percentages
                 *        The population percentage distribution to use.  Each
                 *        value represents the portion of the overall population
                 *        that should be assigned to a group at that index.
                 * @param totalPopulation
                 *        The total amount of population to dispense.  In most
                 *        simulations, this corresponds to the total number of
                 *        agents that should be created.
                 * @param requireAtLeastOne
                 *        Whether or not to force all groups to have at least
                 *        one population, regardless of percentage.  This is
                 *        primarily used in cases where a percentage might be
                 *        incredibly low and for some arbitrary total population
                 *        the resulting floor calculation will be zero.  Thus,
                 *        this is an important way to ensure that all potential
                 *        variations appear at least once in a population of
                 *        significant size.
                 * @throws runtime_error
                 *         If there was a numerical accuracy error of some kind.
                 */
                void initialize(const std::vector<types::fnumeric>& percentages,
                                AgentID totalPopulation,
                                bool requireAtLeastOne);

                /*!
                 * Uses a uniform distribution over the currently available
                 * groups to randomly choose a "type" of population to create
                 * next.
                 *
                 * That sentence is somewhat confusing, but this function
                 * generates the real "output" of random population generation.
                 * For purposes of this project, the "type" of population chosen
                 * corresponds to a single discrete value or behavior chosen
                 * from a set of possible variations.  This function thus
                 * assists in creating the attribute variations of each node in
                 * a socially connected graph of individuals.
                 *
                 * @param random
                 *        The random number generator to use.
                 * @return The next "type" of population to use.
                 * @throws runtime_error
                 *         If the dispenser is empty.
                 */
                types::uint32 nextGroup(types::mersenne_twister& random);

            private:
                /*!
                 * Checks the currently computed population totals against the
                 * required/requested amount and fixes any discrepancies.
                 *
                 * Specifically, this function will determine if the amount of
                 * currently computed population differs from the required
                 * amount.  If so, then a round-robin allocation scheme is used
                 * to shore up the remainder.  This guarantees numerical
                 * accuracy in regards to composition while simultaneously
                 * providing the required amount of population.
                 *
                 * In terms of accuracy, the numerical results should have an
                 * upper bound of two (off by two) but in practice should be
                 * limited to one, as this project does not work with incredibly
                 * small population percentages that might give significant error.
                 *
                 * @param requiredPopulation
                 *        The (accurate) total population required after
                 *        summation.
                 * @throws runtime_error
                 *         If the current population summation is greater than
                 *         the required/requested amount.
                 */
                void checkPopulationRemainder(AgentID requiredPopulation);
                                
            private:
                /*! The vector of currently available group types to choose. */
                std::vector<types::uint32> m_groups;

                /*! The vector current population totals per group. */
                std::vector<types::uint64> m_populations;
        };
    }
}

#endif
