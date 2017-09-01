#ifndef IRIS_MODEL_HPP_
#define IRIS_MODEL_HPP_

#include <fstream>
#include <random>
#include <vector>
#include <string>

#include "iris/Parameters.hpp"
#include "iris/Threading.hpp"
#include "iris/Types.hpp"

#include "iris/io/CommandLine.hpp"
#include "iris/io/reader/CensusReader.hpp"
#include "iris/io/writer/StatisticsWriter.hpp"

namespace iris
{
    // Forward declare to avoid inclusion problems.
    class Agent;

    /*!
     * Represents a mechanism to create, configure, and run a single
     * simulation.
     */
    class Model
    {
        public:
            /*! Constructor. */
            Model();

            /*! Destructor. */
            ~Model();

            /*!
             * Creates a string path to the specified file by combining it with
             * a randomly generated data directory name.
             *
             * @param file
             *        The file to combine the path for.
             * @return A new path to a file in a data directory.
             */
            std::string createPathToData(std::string file) const;

            /*!
             * Creates a string path to the specified file by combining it
             * with the path to the simulation directory.
             *
             * @param file
             *        The file to combine the path for.
             * @return A new path to a file in the simulation directory.
             */
            std::string createPathToParent(std::string file) const;
            
            /*!
             * Configures this simulation using the specified user-input
             * parameters.
             *
             * @param options
             *        A collection of options and arguments parsed from a list
             *        of command line arguments.
             */
            void setUpParams(const io::Options& options);

            /*!
             * Creates and configures all of the agents for this simulation.
             *
             * This also allocates the (shuffled) indice collection.
             */
            void setUpAgents();

            /*!
             * Configures the random number generator to use the specified
             * seed.
             *
             * @param seed
             *        The random seed to use.
             */
            void setUpRandom(types::uint64 seed);

            /*!
             * Creates and configures all of the output streams for this
             * simulation.
             */
            void setUpIoStreams();
      
            /*!
             * Generates a randomized social network for each agent in the
             * simulation, giving rise to a unique graph structure per run.
             */
            void generateGraphStructure();

            /*!
             * Generates randomized value and behavioral attributes for each
             * agent in the simulation, distributed in such a way as to be both
             * random but equitable (the total combinations for each independent
             * discrete variable are equal).
             *
             * In addition, this function also randomly assigns a specific
             * portion of the population to be "powerful".  This quantity is
             * given as an input parameter and is one of the primary influences
             * on the simulation dynamics.
             */
            void generateAttributes();
            
            /*!
             * Runs the simulation for a specific number of time steps.
             */
            void runSimulation();

            /*!
             * Cleans up all allocated resources as necessary.
             */
            void tearDown();

        private:
            
#ifdef IRIS_DEBUG
            /*!
             * Checks the network of every agent for duplicate entries.
             */
            void checkForDuplicates();

            /*!
             * Checks the network of every agent for self-loops (i.e. a network
             * that contains an id that matches the agent who "owns" it).
             */
            void checkForLoops();
#endif
            
        private:
            /*!
             * The array of agents in a simulation.
             *
             * Note that this is a naked pointer array and the total number of
             * elements [in this array] is held in the parameter object.
             */
            Agent*                  m_agents;

            /*!
             * The list of behavioral factors, where each index corresponds to
             * an independent discrete variable.
             */
            BehaviorList            m_behaviors;

            /*!
             * The data from the U.S. Census Bureau concerning the distribution
             * of family sizes per population in America.
             *
             * Please note that this structure is <i>not</i> a probability mass
             * function, not a cumulative distribution function; it is converted
             * behind the scenes from the former to the latter.
             */
            io::CensusData          m_census;

            /*!
             * The collection of parameters that control various aspects of the
             * simulation.
             */
            Parameters              m_params;

            /*!
             * The (core) random number generator.
             */
            types::mersenne_twister m_random;
      
            /*!
             * The list of value factors, where each index corresponds to an
             * independent discrete variable.
             */
            ValueList                  m_values;

        private:
            /*!
             * The parent directory of the experiment file.
             */
            std::string                m_parentDir;

            /*!
             * The directory (created with a unique name) to place the results
             * in.
             */
            std::string                m_dataDir;

            /*!
             * The statistics file stream.
             */
            std::ofstream              m_statsFile;

            /*!
             * The statistics tracker.
             */
            io::StatisticsWriter       m_statistics;

        private:
            /*!
             * The threading controller (unused).
             */
            ThreadController           m_controller;

            /*!
             * The number of threads to use (unused).
             */
            types::uint32              m_numThreads;

            /*!
             * The current time step.
             */
            types::uint64              m_time;

        private:
            /*!
             * The iteration order (shuffled each step).
             */
            std::vector<AgentID>       m_indices;
    };
}

#endif
