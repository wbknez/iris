#ifndef IRIS_PARAMETERS_HPP_
#define IRIS_PARAMETERS_HPP_

#include "iris/Types.hpp"

namespace iris
{
    /*!
     * Represents a collection of parameters used to control various aspects of
     * a simulation.
     */
    // Please note: the ratio Q is intentionally designed to be a 32-bit
    // integer.  This project was not intended to explore massive networks, as
    // such systems would almost certainly be partitioned in some way and we do
    // not currently support that.
    //
    // Formally, the ratio Q is defined as:
    //
    //      # of out-group partners
    //      -----------------------
    //      # of in-group partners
    //
    // per step.  This bounds Q to be between 0 and std::numeric_limits<unsigned
    // int>::max().
    //
    // Finally, note that this project does not define Q as a single quantity.
    struct Parameters
    {
        /*! The parameter argument for the utility function.  */
        types::fnumeric m_lambda;
        
        /*! The number of total agents (vertices) in the simulation. */
        AgentID         m_n;

        /*! The number of non-family connections to make. */
        types::uint32   m_outConnections;

        /*!
         * The percentage of the population that should be deemed
         * "powerful" in a simulation.
         */
        types::fnumeric m_powerPercent;
            
        /*! The number of in-group interactions an agent may have per step. */
        types::uint32   m_qIn;

        /*! The number of out-group interactions an agent may have per step. */
        types::uint32   m_qOut;

        /*!
         * The amount of default "resistance" each agent has by default to
         * changing their behavior after being censored.
         */
        types::fnumeric m_resist;

        /*!
         * The maximum amount of "resistance" each agent may have.
         *
         * Represents the upper bound on the probability of an agent retaining
         * their current behavior.  This is done to reduce chance of stalling,
         * despite the resistance window being quite generous.
         */
        types::fnumeric m_resistMax;

        /*!
         * The minimum amount of "resistance" each agent may have.
         */
        types::fnumeric m_resistMin;

        /*!
         * The maximum number of steps a simulation may execute before stopping.
         */
        types::uint64   m_steps;

        /*!
         * The probability of a connection forming between two agents during
         * graph generation (non-family).
         */
        types::fnumeric m_prob;

        /*!
         * The probability of a reciprocal connection forming between to agents
         * during graph generation (non-family).
         */
        types::fnumeric m_recip;
    };
}

#endif
