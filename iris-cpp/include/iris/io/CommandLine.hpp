/*!
 * Contains mechanisms and structures used to parse and extract options and
 * their arguments from a string obtained via the command line.
 *
 * As noted below, these utilities are simple and designed exclusively for the
 * needs of this project alone.  As such, there are a number of nifty features
 * that are missing (e.g. short options) and are not/will not be included.
 */
#ifndef IRIS_COMMAND_LINE_HPP_
#define IRIS_COMMAND_LINE_HPP_

#include <unordered_map>
#include <string>

#include "iris/Types.hpp"
#include "iris/Utils.hpp"

namespace iris
{
    namespace io
    {
        /*!
         * Represents a collection of parsed command line options and associated
         * arguments.
         */
        class Options
        {
            public:
                typedef std::vector<std::string>                   Arguments;
                typedef std::unordered_map<std::string, Arguments> CommandMap;

                /*! Constructor. */
                Options();

                /*! Destructor. */
                 ~Options();

                /*!
                 * Converts a single argument option to a value of the specified
                 * type.
                 *
                 * @param name
                 *        The name of the option to obtain an argument value
                 *        for.
                 * @return A converted argument value.
                 * @throws runtime_error
                 *         If there is no option with the specified name.
                 */
                template<typename T>
                inline T get(const std::string& name) const;

                /*!
                 * Converts all arguments to a vector of values of the specified
                 * type.
                 *
                 * @param name
                 *        The name of the option to obtain the argument values
                 *        for.
                 * @return A vector of converted argument values.
                 * @throws runtime_error
                 *         If there is no option with the specified name.
                 */
                template<typename T>
                inline std::vector<T> getMultiple(const std::string& name) const;

                /*!
                 * Returns whether or not an option with the specified name was
                 * found in a list of command line arguments.
                 *
                 * This is largely used for determining either existence of
                 * general options or, more specifically, a switch (an option
                 * with 0 arguments).
                 *
                 * @param name
                 *        The name of the option to check for.
                 * @return Whether or not an option is present.
                 */
                bool has(const std::string& name) const;

                /*!
                 * Returns whether or not an option with the specified name was
                 * found in a list of command line arguments with the specified
                 * number of arguments (passed in).
                 *
                 * @param name
                 *        The name of the option to check for.
                 * @param numArgs
                 *        The number of expected arguments.
                 * @return Whether or not an option is present with a specific
                 * argument count.
                 */
                bool has(const std::string& name, types::uint32 numArgs) const;

                /*!
                 * Returns whether or not an option with the specified name was
                 * found in a list of command line arguments and has exactly one
                 * argument.
                 *
                 * @param name
                 *        The name of the option to check.
                 * @return Whether or not an option is present with a single
                 * argument.
                 */
                bool hasSingle(const std::string& name) const;

                /*!
                 * Returns whether or not an option with the specified name was
                 * found in a list of command line arguments and has more than
                 * one argument.
                 *
                 * @param name
                 *        The name of the option to check.
                 * @return Whether or not an option is present with more than
                 * one argument.
                 */
                bool hasMultiple(const std::string& name) const;

                /*!
                 * Adds the specified option to the collection with zero
                 * arguments (also known as a switch).
                 *
                 * @param name
                 *        The name of the option to add.
                 */
                void set(std::string name);

                /*!
                 * Adds the specified option to the collection with the
                 * specified argument.
                 *
                 * @param name
                 *        The name of the option to add.
                 * @param arg
                 *        The (single) argument to associate with an option.
                 */
                void set(std::string name, std::string arg);

                /*!
                 * Adds the specified option to the collection with the
                 * specified vector of arguments.
                 *
                 * @param name
                 *        The name of the option to add.
                 * @param args
                 *        The vector of arguments associated with an option.
                 */
                void set(std::string name, Arguments args);
                
            private:
                /*! The mapping of command line options to arguments. */
                CommandMap m_commands;
        };

        /*!
         * Represents a mechanism for defining and extracting command line
         * options and their expected arguments from an arbitrary string.
         *
         * Because this is a simple project, this is not a full-featured parser.
         * In particular, this parser <b>only</b> processes long-form options;
         * that is, options of the form "--long-option".  Option specification
         * may include an arbitrary number of expected arguments or none at all.
         * Finally, this parser does not do <i>any</i> type inference or
         * conversion; all options and arguments are parsed, collected, and
         * returned as strings.
         *
         * An example command line input string for this project:
         *    ./iris --config sim.cfg --census census.csv --values values.csv
         *           --numThreads 4
         */
        class CommandParser
        {
            public:
                typedef std::unordered_map<std::string, types::uint32> ArgMap;
                typedef std::unordered_map<std::string, std::string>   DescMap;

                typedef std::vector<std::string> Tokens;
                
            public:
                /*! Constructor. */
                CommandParser();

                /*! Destructor. */
                ~CommandParser();

                /*!
                 * Adds a new option specification with the specified name and
                 * description.
                 *
                 * Note that by definition this option is a switch, as there are
                 * no arguments.
                 *
                 * The description is used only for printing a help/usage
                 * message.
                 *
                 * @param name
                 *        The name of the option to add.
                 * @param desc
                 *        The description to use.
                 * @throws runtime_error
                 *         If there was a problem creating or adding the
                 *         specification.
                 */
                void addOption(std::string name, std::string desc);

                /*!
                 * Adds a new option specification with the specified name and
                 * number of expected arguments.
                 *
                 * The description is used only for printing a help/usage
                 * message.
                 *
                 * @param name
                 *        The name of the option to add.
                 * @param numArgs
                 *        The number of arguments to expect.
                 * @param desc
                 *        The description to use.
                 * @throws runtime_error
                 *         If there was a problem creating or adding the
                 *         specification.
                 */
                void addOption(std::string name, types::uint32 numArgs,
                               std::string desc);
               
                /*!
                 * Parses the specified array of command line options into an
                 * auxiliary collection structure that can be queried for
                 * specified values and perform type conversions as necessary.
                 * 
                 * @param argc
                 *        The number of command line arguments, if any.
                 * @param argv
                 *        The list of command line arguments, if any.
                 * @return A collection of parsed options with their values
                 * (if any).
                 * @throws runtime_error
                 *         If there was a problem parsing the options.
                 */
                Options parse(types::uint32 argc, char** argv);
                
                Options parse(Tokens tokens);
                
                
                /*!
                 * Prints a help/usage message to the standard output, where
                 * each option is listed alongside its description.
                 *
                 * @param progName
                 *        The name of the program to use.
                 */
                void printUsage(const std::string& progName);
                
                /*!
                 * Removes the specification for the option with the specified
                 * name.
                 *
                 * @param name
                 *        The name of the option to remove.
                 */
                void removeOption(const std::string& name);

            private:
                /*!
                 * A utility function to tokenize the specified command line
                 * arguments.
                 *
                 * @param argc
                 *        The number of command line arguments.
                 * @param argv
                 *        The command lien arguments to parse.
                 */
                Tokens convertToTokens(int argc, char** argv);
                
                /*!
                 * A utility function to extract all strings from the specified
                 * array of strings between the specified start and end indices,
                 * respectively.
                 *
                 * This is used to collect arguments that immediately follow
                 * options.
                 *
                 * @param start
                 *        The starting index to use.
                 * @param end
                 *        The ending index to use.
                 * @param argv
                 *        The command line arguments to parse.
                 * @return A vector of arguments.
                 */
                 Options::Arguments extractArguments(types::uint32 start,
                                                     types::uint32 end,
                                                     Tokens tokens );

                /*!
                 * A utility function to search an array of strings beginning
                 * from a specified index and locating the next string that has
                 * the format of a long-form command line option (i.e. begins
                 * with "--").
                 *
                 * @param currentIndex
                 *        The index to start from.
                 * @param argc
                 *        The number of command line arguments to parse.
                 * @param argv
                 *        The command line arguments to parse.
                 * @return The next index in the array of a long-form option, or
                 * <i>argc</i> if none were found.
                 */
                types::uint32 findNextOption(types::uint32 currentIndex,
                                             const Tokens& tokens);
                
            private:
                /*!
                 * The mapping of options to their expected number of
                 * arguments.
                 */
                ArgMap  m_args;

                /*! The mapping of options to their descriptions.  */
                DescMap m_desc;
        };

        /*!
         * Returns the value of the option with the specified name, if any is
         * found.
         *
         * @param name
         *        The option to search for.
         * @return The value of the option with a given name.
         */
        template<typename T>
        T Options::get(const std::string& name) const
        {
            const auto found = m_commands.find(name);

            if(found == m_commands.end())
            {
                throw std::runtime_error("Could not find: " + name);
            }

            // Technically, this requires a state check as well.  But since this
            // is a simple parser that I explicitly control, it is not a big
            // deal.
            Arguments args = found->second;
            return util::parseString<T>(args[0]);
        }

        /*!
         * Returns all values associated with the option with the specified
         * name.
         *
         * @param name
         *        The option to search for.
         * @return All values of the option with a given name.
         */
        template<typename T>
        std::vector<T> Options::getMultiple(const std::string& name) const
        {
            const auto found = m_commands.find(name);

            if(found == m_commands.end())
            {
                throw std::runtime_error("Could not find: " + name);
            }

            // Again, this should require a check but it's fine in this
            // instance.
            //
            // The vector of type-converted arguments.
            std::vector<T> converted;

            for(const auto arg : found->second)
            {
                const T elem = util::parseString<T>(arg);
                converted.push_back(elem);
            }

            return converted;
        }
    }
}

#endif
