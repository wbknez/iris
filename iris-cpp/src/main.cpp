#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

#include "iris/Model.hpp"
#include "iris/Types.hpp"
#include "iris/Utils.hpp"

#include "iris/io/CommandLine.hpp"

/*!
 * Creates the command line parser by adding specifications for the types of
 * options this project expects to see.
 *
 * @param argc
 *        The number of command line arguments, if any.
 * @param argv
 *        The list of command line arguments, if any.
 */
iris::io::CommandParser createCommandParser();

/*!
 * The main driver of the Iris project.
 *
 * @param argc
 *        The number of command line arguments, if any.
 * @param argv
 *        The list of command line arguments, if any.
 */
int main(int argc, char** argv)
{
    // Terminal color sequences.
    iris::util::term::Sequence red(iris::util::term::Color::Red);
    iris::util::term::Sequence def(iris::util::term::Color::Default);
    
    // The command line arguments are as follows:
    //    [directory] [threads]
    // of the form:
    //    [path] [uint]
    iris::io::CommandParser parser;
    iris::io::Options       options;

    try
    {
        parser  = createCommandParser();
        options = parser.parse(argc, argv);
    }
    catch(std::runtime_error& re)
    {
        std::cerr << red << "*" << def << "Command Line Error (aborting)"
                  << std::endl;
        std::cerr << "What happened: " << re.what() << std::endl;
        return 0;
    }

    // Generate the seed for the (core) random number generator.
    const auto currentTime = std::chrono::high_resolution_clock::now();
    const auto currentSeed = currentTime.time_since_epoch().count();
    
    // The model itself.
    iris::Model model;

    try
    {
        // Configure the model.
        model.setUpParams(options);
        model.setUpAgents();
        model.setUpRandom(currentSeed);

        // Generate the graph (wire up family units => friends outside).
        model.generateGraphStructure();
        model.generateAttributes();

        // Set up streaming.
        model.setUpIoStreams();
    }
    catch(std::runtime_error& re)
    {
        std::cerr << red << "*" << def << " Generation Error (aborting)"
                  << std::endl;
        std::cerr << "What happened: " << re.what() << std::endl;
        return 0;
    }
    
    try
    {      
        // Set up the threading model.
        //        model.setUpThreading();
      
        // Run the simulation.
        model.runSimulation();

        // Clean up.
        model.tearDown();
    }
    catch(std::runtime_error& re)
    {
        std::cerr << red << "*" << def << " Simulation error (aborting)"
                  << std::endl;
        std::cerr << "What happened: " << re.what() << std::endl;
        return 0;
    }
}

iris::io::CommandParser createCommandParser()
{
    // Set up the parser itself.
    iris::io::CommandParser parser;

    parser.addOption("directory", 1, "The directory containing simulation"
                                     " data files.");
    parser.addOption("run", 1, "The current simulation run.");

    return parser;
}
