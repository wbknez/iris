/*!
 * Contains a mechanism to read and parse a CSV (comma separated value) file of
 * integer values into two vectors, each of which describe a set of discrete
 * variables used in a simulation.
 */
#ifndef IRIS_VALUE_READER_HPP_
#define IRIS_VALUE_READER_HPP_

#include <istream>
#include <string>
#include <utility>
#include <vector>

#include "iris/Model.hpp"

namespace iris
{
    namespace io
    {
        typedef std::pair<ValueList, BehaviorList> ValueParams;

        /*!
         * Checks whether or not the value list is strictly less than the size
         * of the behavior list and, if so, throws an exception to alert the
         * caller of this.
         *
         * @throws runtime_error
         *         If the size of the value list is (strictly) less than the
         *         size of the behavior list.
         */
        void enforceSizeRequirement(const ValueParams& params);

        /*!
         * Checks whether or not the values of both the behavior list and the
         * value list match.
         *
         * Please note that this method checks <i>only</i> the intersection of
         * these two lists.  Because the behavior list is designed to be
         * derivative of the values - that is, the length of the behavior list
         * is required to be less than or equal to that of the list of values -
         * the non-intersecting portion of the values list is completely
         * irrelevant.
         *
         * @throws runtime_error
         *         If there is a discrepancy between the behavior and value
         *         lists.
         */
        void enforceValueRequirements(const ValueParams& params);
        
        /*!
         * Reads all of the lines in a stream and returns them as a vector of
         * strings for processing.
         *
         * As should be obvious, this method is only particularly useful for
         * small streams.
         *
         * @param in
         *        The stream to read.
         * @return A vector of lines read.
         */
        std::vector<std::string> getLines(std::istream& in);

        /*!
         * Reads the specified CSV (command separated value) file and extracts
         * the set of discrete values and behaviors to use in a simulation.
         *
         * @param filename
         *        The name of the CSV file to read (and parse).
         * @return A set of values and behaviors to use.
         * @throws runtime_error
         *         If the file could not be read or parsed correctly.
         */
        ValueParams readValuesData(const std::string& filename);

        /*!
         * Parses the specified stream in order to extract the set of values and
         * behaviors to use in a simulation.
         *
         * The expected format of the stream is CSV (comma separated value) and
         * an example is:
         *    3,4,2,2
         *    3,4
         * where the first line denotes the number of discrete values (per
         * column) and the second is the number of behaviors.  Please keep in
         * mind that the number of behaviors must be less than or equal to the
         * values.  Furthermore, the second line is optional; to specify that
         * the behaviors and values are bijective simply do not include it.
         *
         * Finally, the numeric values themselves are assumed to be 8-bit and
         * unsigned.  Anything larger will cause an error during processing.
         *
         * @param filename
         *        The name of the file to read the values from.
         * @return The set of values and behaviors to use.
         * @throws runtime_error
         *         If the stream is not formatted correctly.
         */
        ValueParams parseValuesFromCSV(std::istream& in);
    }
}

#endif
