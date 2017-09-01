/*!
 * Contains mechanisms to create a randomized urban social network.
 *
 * First, please note that this is <b>neither</b> a comprehensive nor
 * necessarily accurate depiction of social urbanization.  Rather, this
 * methodology was created in response to the fact that almost all current graph
 * algorithms are designed for networks produced online - e.g. Barabasi-Albert,
 * Watts-Strogatz, etc. - which are characterized as power-law abiding, scale
 * free networks.  Many of these algorithms use preferential attachment and tend
 * to generate heavy-tailed networks, which is unfortunate because this results
 * in a large number of agents having no social networks at all (since they are
 * leaves).  The degree distributions of these networks are also quite
 * disturbing, since preferential attachment will generate networks whose
 * degrees between "layers" are vastly different, making it difficult to create
 * uniformly dense networks.  Finally, it is very difficult to obtain a specific
 * type of degree structure without excessive parameter tweaking.
 *
 * Instead, the method presented here seeks to create an urban social network
 * using family units as a base.  In short, the resultant network is a series of
 * randomly connected sub-components, where each sub-component represents a
 * family whose members are all socially connected to each other.  Thus, network
 * formation is divided into two steps: (1) family generation, and (2)
 * friendship generation.  Family generation is controlled by U.S. Census Bureau
 * data that breaks down the percentage of American families that have [1-8]
 * members.  This data is used to produce a cumulative distribution function
 * (CDF) used to determine graph topology.  This ensures that each graph
 * produced by this project is unique but whose topology is both known and
 * structurally similar within some tolerance.  Friendship generation follows a
 * Binomial distribution based on the probability of one agent bonding to
 * another, up to a maximum number of additional non-family connections that may
 * be made.  Reciprocity is controlled by an additional probability parameter,
 * thus graph topology may be further varied without structural changes
 * (i.e. family data variation).
 */
#ifndef IRIS_GRAPH_GENERATOR_HPP_
#define IRIS_GRAPH_GENERATOR_HPP_

#include <utility>
#include <vector>

#include "iris/Agent.hpp"
#include "iris/Types.hpp"

#include "iris/io/reader/CensusReader.hpp"

namespace iris
{
    namespace gen
    {
        typedef std::vector<types::fnumeric>     CDF;
        typedef std::pair<AgentID, AgentID>      FamilyUnit;
        
        /*!
         * Randomly selects the size of a family unit from the specified CDF
         * (created from U.S. Census data).
         *
         * @param p
         *        The probability of a connection being made between two agents.
         * @param cdf
         *        The cumulative distribution function that represents the
         *        distribution of family sizes across an agent population.
         * @return The size of a randomly selected family unit.
         * @throws runtime_error
         *         If no valid value was obtained from the CDF.
         */
        types::uint32 chooseFamilySize(const types::fnumeric& p,
                                       const CDF& cdf);

        /*!
         * Creates a cumulative distribution function from the specified census
         * data.
         *
         * This function essentially turns the following list of example data:
         *    0.15, 0.26, 0.12, 0.13, 0.23
         * into:
         *    0.15, 0.42, 0.64, 0.77, 1.0
         * which provides the mechanism for choosing random value sizes.
         *
         * @param census
         *        The census data to use.
         * @return A cumulative distribution function based on the specified
         * data.
         */
        CDF createCDF(const io::CensusData& census);
        
        /*!
         * Wires all of the agents that belong to the specified family unit to
         * the specified agent.
         *
         * @param agent
         *        The agent to wire to.
         * @param unit
         *        The family unit the agent belongs to.
         */
        void wireFamilyUnit(Agent& agent, const FamilyUnit& unit);
        
        /*!
         * Wires the specified list of agents to form an interconnected network
         * of family units.
         *
         * Specifically, this function uses the specified census data to create
         * blocks of "family units" of various sizes.  Each agent in a family is
         * then allowed to connect to an additional maximum number of agents
         * outside the family.  These connections are based on the specified
         * parameter and occur with the specified probability, essentially using
         * a Binomial distribution to create the additional out-going network.
         * Finally, non-family connections are reciprocal in nature with some
         * additional specified probability.
         *
         * @param agents
         *        The list of agents.
         * @param totalAgents
         *        The total number of agents in the simulation.
         * @param census
         *        The census data to use.
         * @param outConnections
         *        The maximum number of non-family connections that may be made.
         * @param connectionProb
         *        The probability that a non-family connection will be made.
         * @param recipProb
         *        The probability that a non-family connection is reciprocated.
         * @param random
         *        The random number generator to use.
         */
        void wireGraph(Agent* const agents, AgentID totalAgents,
                       const io::CensusData census,
                       types::uint32 outConnections,
                       types::fnumeric connectionProb,
                       types::fnumeric recipProb,
                       types::mersenne_twister& random);

        /*!
         * Wires the specified agent randomly to a maximum of other agents.
         *
         * There are a number of parts to this process.  First, the randomly
         * selected agents are not from the family unit.  Second, each
         * connection is a potential connection that may occur with the
         * specified probability.  Third, links are considered reciprocal with
         * an additional specified probability parameter.
         *
         * @param id
         *        The agent to wire.
         * @param agents
         *        The list of agents.
         * @param totalAgents
         *        The total number of agents in the simulation.
         * @param outConnections
         *        The maximum number of non-family connections that may be made.
         * @param unit
         *        The family unit of an agent.
         * @param connectionProb
         *        The probability that a non-family connection will be made.
         * @param recipProb
         *        The probability that a non-family connection is reciprocated.
         * @param random
         *        The random generator to use.
         */
        void wireOutGroup(const AgentID& id, Agent* const agents,
                          AgentID totalAgents, types::uint32 outConnections,
                          types::fnumeric connectionProb,
                          types::fnumeric recipProb,
                          types::mersenne_twister& random);
    }
}

#endif
