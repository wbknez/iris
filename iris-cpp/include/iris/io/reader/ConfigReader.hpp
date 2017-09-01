/*!
 * Contains mechanism(s) to read and parse an arbitrary number of key/value
 * pairs from a simple configuration file (.cfg).
 */
#ifndef IRIS_CONFIG_READER_HPP_
#define IRIS_CONFIG_READER_HPP_

#include <istream>
#include <unordered_map>
#include <string>

namespace iris
{
    namespace io
    {
        typedef std::unordered_map<std::string, std::string> Config;

        /*!
         * Reads the specified configuration file.
         *
         * @param filename
         *        The name of the CFG file to read (and parse).
         * @return A map of key/value pairs representing configuration options.
         * @throws runtime_error
         *         If there was a problem reading the file.
         */
        Config readConfiguration(const std::string& filename);
        
        /*!
         * Parses the specified configuration stream and places all key/value
         * pairs into a simple un-ordered map.
         *
         * The syntax of this configuration file is the simple kind seen in most
         * Linux config files (e.g. .cfg).  The resultant map is simply a map of
         * strings, there is no attempt to infer or convert to custom types
         * (that is the responsibility of the caller).  Comments are supported
         * in the file with '#' as expected.
         *
         * @param in
         *        The CFG stream to parse.
         * @return A map of key/value strings.
         */
        Config parseConfigurationFromCFG(std::istream& in);
    }
}

#endif
