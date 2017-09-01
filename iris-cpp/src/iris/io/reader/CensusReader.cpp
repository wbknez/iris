#include "iris/io/reader/CensusReader.hpp"

#include <fstream>
#include <regex>
#include <stdexcept>

#include "iris/Utils.hpp"

namespace iris
{
    namespace io
    {
        CensusData readCensusData(const std::string& filename)
        {
            std::ifstream infile(filename);

            if(!infile.is_open())
            {
                throw std::runtime_error("Could not read CSV file: " +
                                         filename);
            }

            auto census = parseCensusDataFromCSV(infile);
            infile.close();
            return census;
        }
        
        CensusData parseCensusDataFromCSV(
            std::istream& in)
        {
            typedef std::sregex_token_iterator tokenizer;
            typedef std::vector<std::string>   tokens;
            
            std::string line;
            const std::regex pattern("[,]+");
            CensusData census;

            std::getline(in, line);
            line = util::trim(line);

            tokens read = {tokenizer{line.begin(), line.end(), pattern, -1},
                           tokenizer{}};

            for(auto token : read)
            {
                const auto value = util::parseString<types::fnumeric>(token);
                census.push_back(value);
            }
            
            return census;
        }
    }
}
