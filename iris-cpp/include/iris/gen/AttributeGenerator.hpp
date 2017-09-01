/*!
 * Contains mechanisms to create populations of agents, where each individual is
 * differentiated from others by their possession of an arbitrary combination of
 * discrete variables.
 */
#ifndef IRIS_ATTRIBUTE_GENERATOR_HPP_
#define IRIS_ATTRIBUTE_GENERATOR_HPP_

#include <string>
#include <vector>

#include "iris/Agent.hpp"
#include "iris/Types.hpp"

#include "iris/gen/PopulationDispenser.hpp"

namespace iris
{
    namespace gen
    {
        typedef std::vector<std::string>         PermuteList;
        typedef std::vector<PopulationDispenser> PopDispensers;

        /*!
         * Converts the specified list of independent discrete variables to a
         * single integer type.
         *
         * @param list
         *        The list of variables to convert.
         * @return A list converted to an integer.
         */
        types::uint32 convertListToInteger(const Uint32List& list);

        /*!
         * Converts the specified list of independent discrete variables to a
         * string representing a single integer type.
         *
         * @param list
         *        The list of variables to convert.
         * @return A list converted to a string.
         */
        std::string convertListToString(const Uint32List& list);
        
        /*!
         * Creates a randomized list of discrete variables that represents a
         * single individual in a population.
         *
         * The dispenser vector contains a population dispenser for each "type",
         * or category, of discrete variable in the simulation.  Thus, each
         * index into the resultant list is a randomized discrete variable drawn
         * from independent distributions.  The combination of these discrete
         * variables forms the differentiating basis for each individual agent
         * in a population.
         *
         * @param dispensers
         *        The collection of variable randomizers.
         * @param random
         *        The random generator to use.
         * @return A list of random discrete variables.
         */
        Uint32List createAttributeList(PopDispensers& dispensers,
                                       types::mersenne_twister& random);

        /*!
         * Creates a list of population dispensers that combined are capable of
         * generating a set number of combinations of an independent set of
         * discrete variables.
         *
         * @param vars
         *        The list of discrete variables to draw from, where each index
         *        represents a distinct independent set of those variables.
         * @param totalAgents
         *        The total number of agents in the simulation.
         * @return A list of population dispensers, each a unique dispenser of
         * an independent set of discrete variables.
         */
        PopDispensers createPopulationDispensers(Uint32List vars,
                                                 AgentID totalAgents);

        /*!
         * Creates a sub list that is a strict subset of the specified base list
         * but of the same length as the target.
         *
         * Obviously, there is no need to pass two lists.  However, it is
         * convenient in order to ensure the size property as well as enforce
         * the concept.
         *
         * @param base
         *        The base to copy.
         * @param target
         *        The list to derive the size from.
         * @return A sub-list of a base list.
         * @throws runtime_error
         *         If the size of the base list is strictly less than the target
         *         list.
         */
        Uint32List createSubAttributeList(const Uint32List& base,
                                          const Uint32List& target);
        
        /*!
         * Iterates over the specified array of agents and assigns each one a
         * randomized combination of discrete variables as starting values and
         * behaviors.
         *
         * In brief, this function takes two sets of independent discrete
         * variables as input representing values and behaviors, respectively,
         * where each element represents the number of factors per variable, and
         * distributes them evenly over a population of agents.  There is no
         * weighting, no preference, and no heed paid to the location of the
         * agent assigned a certain attribute combination.
         *
         * @param agents
         *        A collection of agents.
         * @param totalAgents
         *        The total number of agents in a simulation.
         * @param values
         *        The vector of value factors.
         * @param behaviors
         *        The vector of behavior factors.
         * @param random
         *        The random generator to use.
         * @throws runtime_error
         *         If the behavior specification is greater than the values;
         *         that is, if either the length of the vectors are different
         *         or if each behavior element is greater than the corresponding
         *         values element.
         */
        void generateAttributes(Agent* const agents, AgentID totalAgents,
                                ValueList values, BehaviorList behaviors,
                                types::mersenne_twister& random);

        /*!
         * Randomly assigns a specified portion of an agent population to be
         * "powerful".
         *
         * In terms of this project, the term "powerful" has a specific impact
         * on the simulation dynamics.  Thus, the number of agents in a
         * simulation that are considered to be powerful is given as an input
         * parameter.
         *
         * @param agents
         *        A collection of agents.
         * @param totalAgents
         *        The total number of agents in a simulation.
         * @param powerPercent
         *        The percentage of a total population that should be deemed
         *        "powerful" for purposes of simulation dynamics.
         * @param requireAtLeastOne
         *        Whether or not to force at least one powerful agent to exist
         *        if the requested percentage yields an effectively zero
         *        population.  This is useful/required for simulations that
         *        use small agent populations and desire correspondingly small
         *        power agent percentages.
         * @param random
         *        The random generator to use.
         */
        void generatePowerfulAgents(Agent* const agents, AgentID totalAgents,
                                    types::fnumeric powerPercent,
                                    bool requireAtLeastOne,
                                    types::mersenne_twister& random);

        /*!
         * Creates a list of string permutations of the specified variable list.
         *
         * @param vars
         *        The list of independent discrete variables to permute.
         * @return A list of string permutations.
         */
        PermuteList permuteList(const Uint32List& vars);

        /*!
         * Creates a single permutation of the specified list of variables and
         * adds it to a list.
         *
         * Technically, the above only occurs if the specified index is equal to
         * one minus the size of the variable list, thereby denoting the end of
         * a single permutation.  Otherwise, this function calls itself
         * recursively until the prior condition is reached.
         *
         * @param vars
         *        The list of independent discrete variables to permute.
         * @param permutes
         *        The list of created permutations.
         * @param mutableList
         *        The current permutation.
         * @param index
         *        The current index (or depth) in a single permutation.
         */
        void permuteList(const Uint32List& vars, PermuteList& permutes,
                         Uint32List& mutableList, types::uint32 index);
    }
}

#endif
