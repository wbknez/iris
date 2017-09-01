#include "iris/Model.hpp"

#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>

#include "iris/Agent.hpp"
#include "iris/Utils.hpp"
#include "iris/Types.hpp"

#include "iris/gen/AttributeGenerator.hpp"
#include "iris/gen/GraphGenerator.hpp"

#include "iris/io/CommandLine.hpp"

#include "iris/io/reader/ConfigReader.hpp"
#include "iris/io/reader/ValueReader.hpp"

#include "iris/io/writer/AttributeWriter.hpp"
#include "iris/io/writer/CommWriter.hpp"
#include "iris/io/writer/NetworkWriter.hpp"
#include "iris/io/writer/PowerWriter.hpp"

namespace iris
{
    Model::Model()
    : m_agents(NULL)
    {}

    Model::~Model()
    {}

#ifdef IRIS_DEBUG    
    void Model::checkForDuplicates()
    {        
      for(AgentID i = 0; i < m_params.m_n; i++)
        {
            const auto network = m_agents[i].getNetwork();
            const auto id      = m_agents[i].getUId();

            if(network.size() == 0)
            {
                continue;
            }

            for(Agent::Network::size_type j = 0; j < (network.size() - 1); j++)
            {                
                if(network[j] == network[j + 1])
                {
                    throw std::runtime_error("Duplicate found at: " +
                                             util::toString(id) +
                                             " with " +
                                             util::toString(network[j]));
                }
            }
        }
    }

    void Model::checkForLoops()
    {
      for(auto i = (AgentID)0; i < m_params.m_n; i++)
        {
            const auto network = m_agents[i].getNetwork();

            if(std::find(network.begin(), network.end(), i) != network.end())
            {
                throw std::runtime_error("Loop found at: " + util::toString(i));
            }
        }
    }
#endif

    std::string Model::createPathToData(std::string file) const
    {
        return m_dataDir + "/" + file;
    }

    std::string Model::createPathToParent(std::string file) const
    {
        return m_parentDir + "/" + file;
    }
    
    void Model::generateGraphStructure()
    {
        gen::wireGraph(m_agents, m_params.m_n, m_census,
                       m_params.m_outConnections,
                       m_params.m_prob, m_params.m_recip,
                       m_random);

#ifdef IRIS_DEBUG
        this->checkForDuplicates();
        this->checkForLoops();
#endif
    }

    void Model::generateAttributes()
    {
        gen::generateAttributes(m_agents, m_params.m_n, m_values, m_behaviors,
                                m_random);
        gen::generatePowerfulAgents(m_agents, m_params.m_n,
                                    m_params.m_powerPercent, true, m_random);
    }
    
    void Model::setUpParams(const io::Options& options)
    {
        using namespace iris::io;
        using namespace iris::types;
        using namespace iris::util;

        // Which run is this?
        const auto run = options.get<types::uint32>("run");
        
        // Set up the directory structure, first.
        m_parentDir = options.get<std::string>("directory");
        m_dataDir   = createDataDirectory(m_parentDir, run);
        
        // Obtain the file names.
        const auto censusFilename = this->createPathToParent("census.csv");
        const auto paramsFilename = this->createPathToParent("params.cfg");
        const auto valuesFilename = this->createPathToParent("values.csv");
        
        // Set up the census first.
        m_census = readCensusData(censusFilename);

        // Then the values and behaviors.
        ValueParams valueParams = readValuesData(valuesFilename);

        m_behaviors = valueParams.second;
        m_values    = valueParams.first;

        // Set up the parameters after.
        Config config = readConfiguration(paramsFilename);

        // Convert all values as necessary.
        m_params.m_lambda = parseString<fnumeric>(config["lambda"]);
        m_params.m_n = parseString<AgentID>(config["n"]);
        m_params.m_outConnections = parseString<uint32>(config["outConn"]);
        m_params.m_powerPercent = parseString<fnumeric>(config["powerPercent"]);
        m_params.m_qIn = parseString<uint32>(config["qIn"]);
        m_params.m_qOut = parseString<uint32>(config["qOut"]);
        m_params.m_resist = parseString<fnumeric>(config["resist"]);
        m_params.m_resistMax = parseString<fnumeric>(config["resistMax"]);
        m_params.m_resistMin = parseString<fnumeric>(config["resistMin"]);
        m_params.m_steps = parseString<uint64>(config["maxSteps"]);
        m_params.m_prob = parseString<fnumeric>(config["linkProb"]);
        m_params.m_recip = parseString<fnumeric>(config["recipProb"]);
    }

    void Model::setUpAgents()
    {
        if(m_agents)
        {
            throw std::runtime_error("Agents have already been initialized!");
        }
        
        // Create the array and then assign correct ids.
        //
        // Because we are using a single dynamic array for this, there is no
        // other way to do so.
        m_agents = new Agent[m_params.m_n];

        for(auto i = (AgentID)0; i < m_params.m_n; i++)
        {
            m_agents[i].setUId(i);
        }

        m_indices.resize(m_params.m_n);
        std::iota(m_indices.begin(), m_indices.end(), 0);
    }
    
    void Model::setUpRandom(types::uint64 seed)
    {
        m_random = types::mersenne_twister(seed);
    }

    void Model::setUpIoStreams()
    {
        using namespace iris::io;
        
        // Write the initial graph data (for posterity).
        //
        // We name it something different than normal (e.g. not vertices-0.csv)
        // to make it easy to find.
        writeAttributes(this->createPathToData("original-attributes.csv"),
                        m_agents, m_params.m_n);
        writeNetwork(this->createPathToData("original-network.csv"),
                     m_agents, m_params.m_n);
        
        // Set up the (running) statistics file.
        m_statsFile = std::ofstream(this->createPathToData("statistics.csv"));

        // Disable the separator.
        m_statsFile.imbue(std::locale(m_statsFile.getloc(),
                                      new disable_thousands_sep()));
        
        m_statistics.initialize(m_behaviors);
        m_statistics.writeHeader(m_statsFile);
        m_statistics.writeStatistics(m_statsFile, m_agents, m_params.m_n, 0);
    }

    void Model::runSimulation()
    {
        using namespace iris::types;

        // The current time step.
        m_time = 0;

        while(m_time < m_params.m_steps)
        {
            m_time++;
            
#ifdef IRIS_DEBUG
            std::cout << "Starting time: " << m_time << std::endl;
            std::cout << "Signaling workers from main thread." << std::endl;
#endif
            std::shuffle(m_indices.begin(), m_indices.end(), m_random);

            for(auto& ind : m_indices)
            {
                m_agents[ind].step(m_params, m_agents, m_params.m_n,
                                   m_behaviors, m_time, m_random);
            }
            
#ifdef IRIS_DEBUG
            std::cout << "Done waiting for workers." << std::endl;
#endif
            // Write out to (cumulative) statistics file.
            m_statistics.writeStatistics(m_statsFile, m_agents,m_params.m_n,
                                         m_time);
#ifdef IRIS_DEBUG
            std::cout << "Finishing time: " << m_time << std::endl;
#endif
        }
    }
    
    void Model::tearDown()
    {
        using namespace iris::io;
        
#ifdef IRIS_DEBUG
        std::cout << "Tearing down." << std::endl;
#endif
        writeAttributes(this->createPathToData("final-attributes.csv"),
                        m_agents, m_params.m_n);
        writeComm(this->createPathToData("comm.csv"), m_agents,
                  m_params.m_n, m_time);
        writePower(this->createPathToData("power.csv"), m_agents,
                   m_params.m_n);
      
        if(m_agents)
        {
            delete[] m_agents;
            m_agents = NULL;
        }

        if(m_statsFile.is_open())
        {
            m_statsFile.close();
        }
    }
}
