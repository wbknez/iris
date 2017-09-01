#ifndef IRIS_STATISTICS_WRITER_HPP_
#define IRIS_STATISTICS_WRITER_HPP_

#include <locale>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "iris/Types.hpp"

#include "iris/gen/AttributeGenerator.hpp"

namespace iris
{
    class Agent;
    
    namespace io
    {
        /*!
         * Returns the current date and time in string form.
         *
         * Specifically, the string is formatted as follows:
         *    %Y%M%D-%H%M
         * or, as an example:
         *    20161117-1644
         * which serves as a unique identifier for a data directory.
         *
         * @return The date and time formatted as a string.
         */
        std::string getDateAndTime();

        /*!
         * Creates the (data) directory in which the currently running
         * simulation will place all of its data files.
         *
         * @param where
         *        The parent directory to use.
         * @param run
         *        The current simulation run (to serve as a unique identifier).
         */
        std::string createDataDirectory(const std::string& where,
                                        types::uint32 run);

        struct disable_thousands_sep : std::numpunct<char>
        {
            char_type do_thousands_sep() const { return '\0'; }
        };
        
        /*!
         * Represents a mechanism for managing and writing certain kinds of
         * statistics to a stream repeatedly over the lifespan of a simulation.
         */
        class StatisticsWriter
        {
            public:             
                typedef std::unordered_map<std::string, types::uint32> CensusMap;
                
                /*! Constructor. */
                StatisticsWriter();

                /*! Destructor. */
                ~StatisticsWriter();

                /*!
                 * Clears the census of current tabulations.
                 *
                 * This is always called before every write (and before
                 * processing the current time step).
                 */
                void clear();

                /*!
                 * Discovers all possible permutations of the specified behavior
                 * variable(s) and prepares the census collection for quick
                 * processing.
                 *
                 * @param behavior
                 *        The list of behavior(s) to use.
                 */
                void initialize(const Uint32List& behavior);

                /*!
                 * Writes a CSV (comma separated value) header concerning the
                 * types of data this statistics writer will produce to the
                 * specified stream.
                 *
                 * @param out
                 *        The stream to write to.
                 */
                void writeHeader(std::ostream& out);
                
                /*!
                 * Collects both the current total amount of privilege possessed
                 * by all agents in a simulation and histogram data on current
                 * behavior composition, before writing this information to the
                 * specified stream.
                 *
                 * @param out
                 *        The stream to write to.
                 * @param agents
                 *        The list of agents.
                 * @param totalAgents
                 *        The total number of agents in a simulation.
                 * @aram currentTime
                 *       The current time step.
                 */
                void writeStatistics(std::ostream& out,
                                     Agent* const agents,
                                     AgentID totalAgents,
                                     types::uint64 currentTime);
                
            private:
                /*!
                * Represents the current number of agents that match a particular behavior permutation.
                *
                * This map is cleared (reset to zero) before every write (before processing).
                */
                CensusMap         m_census;

                /*!
                 * Represents both a collection of permutations for a given set
                 * of behaviors but also denotes the order in which the behavior
                 * statistics will be written out, as it is important to do so
                 * in the exact same way across the lifespan of a simulation.
                 */
                gen::PermuteList  m_permutes;
        };
    }
}

#endif
