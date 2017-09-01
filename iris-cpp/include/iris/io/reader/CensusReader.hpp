/*!
 * Contains a mechanism to read and parse a CSV (comma separated value) file of
 * floating point values into a vector, where each value represents U.S. census
 * data describing family size as a percentage of population.
 */
#ifndef IRIS_CENSUS_READER_HPP_
#define IRIS_CENSUS_READER_HPP_

#include <istream>
#include <string>
#include <vector>

#include "iris/Types.hpp"

namespace iris
{
    namespace io
    {
        typedef std::vector<types::fnumeric> CensusData;

        /*!
         * Reads census data from a file formatted as a CSV (comma separated
         * values).
         *
         * @param filename
         *        The name of the CSV file to read (and parse).
         * @return A vector of floating-point percentages representing family
         * size by percentage.
         * @throws runtime_error
         *         If there was a problem reading the file.
         */
        CensusData readCensusData(const std::string& filename);
        
        /*!
         * Parses census data from a stream formatted as a CSV (comma separated
         * value) file.
         *
         * The stream itself contains U.S. census data relating to family size
         * by population.  That is, each entry corresponds to the percentage of
         * the U.S. that has, or at least identifies as having, a family of a
         * specific size.  An example:
         *    0.16,0.3,0.3,0.12,0.15
         * corresponds to a set of data that says that 16% of U.S. families are
         * single-unit, 30% consist of two individuals, 30% have three people,
         * 12% have four, and 15% are a family of five, respectively.
         *
         * It is worth noting that the returned vector is indexed at 0, but the
         * first (0th) element still refers to a family size of one.
         *
         * @param in
         *        The CSV stream to parse.
         * @return A vector of percentages for family sizes.
         */
        CensusData parseCensusDataFromCSV(std::istream& in);
    }
}

#endif
