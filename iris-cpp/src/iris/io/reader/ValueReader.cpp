#include "iris/io/reader/ValueReader.hpp"

#include <fstream>
#include <regex>
#include <stdexcept>
#include <vector>

#include "iris/Types.hpp"
#include "iris/Utils.hpp"

namespace iris
{
    namespace io
    {
        void enforceSizeRequirement(const ValueParams& params)
        {
            if(params.first.size() < params.second.size())
            {
                throw std::runtime_error("Size requirement violated: value list"
                                         " is smaller than the list of"
                                         " behaviors!");
            }
        }

        void enforceValueRequirements(const ValueParams& params)
        {
            for(BehaviorList::size_type i = 0; i < params.second.size(); i++)
            {
                if(params.first[i] != params.second[i])
                {
                    throw std::runtime_error("Values do not match behaviors"
                                             " (lack of bijectivity): " +
                                             util::toString(params.first[i]) +
                                             " != " +
                                             util::toString(params.second[i]) +
                                             "!");
                }
            }
        }
        
        std::vector<std::string> getLines(std::istream& in)
        {
            std::string line;
            std::vector<std::string> lines;

            while(std::getline(in, line))
            {
                lines.push_back(line);
            }

            return lines;
        }

        ValueParams readValuesData(const std::string& filename)
        {
            std::ifstream infile(filename);

            if(!infile.is_open())
            {
                throw std::runtime_error("Could not open CSV file: " + filename);
            }

            ValueParams values = parseValuesFromCSV(infile);
            infile.close();
            return values;
        }
        
        ValueParams parseValuesFromCSV(std::istream& in)
        {
            typedef std::sregex_token_iterator tokenizer;
            typedef std::vector<std::string>   tokens;

            const std::regex pattern("[,]+");
            const std::vector<std::string> lines = getLines(in);
            ValueParams result;

            if(lines.size() == 0)
            {
                throw std::runtime_error("Values file has nothing in it!");
            }

            if(lines.size() > 2)
            {
                throw std::runtime_error("Values file is not formatted "
                                         "correctly as a CSV file!");
            }
            
            // Parse the input.
            //
            // There should be *only* two lines maximum.
            const auto valueStr = util::trim(lines[0]);
            tokens valueTokens = {tokenizer{valueStr.begin(), valueStr.end(),
                                            pattern, -1},
                                  tokenizer{}};
            
            for(const auto value : valueTokens)
            {
                const auto v = util::parseString<types::uint32>(value);
                result.first.push_back(v);
            }

            // The second line is optional.
            if(lines.size() == 2)
            {
                const auto behavStr = util::trim(lines[1]);
                tokens behavTokens = {tokenizer{behavStr.begin(),
                                                behavStr.end(),
                                                pattern, -1},
                                      tokenizer{}};

                for(const auto behav : behavTokens)
                {
                    const types::uint8 b =
                        util::parseString<types::uint32>(behav);
                    result.second.push_back(b);
                }

                enforceSizeRequirement(result);
                enforceValueRequirements(result);
            }
            else
            {
                result.second = result.first;
            }

            return result;
        }
    }
}
