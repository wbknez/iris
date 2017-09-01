#include "iris/io/reader/ConfigReader.hpp"

#include <fstream>
#include <regex>
#include <stdexcept>
#include <vector>

#include "iris/Utils.hpp"

namespace iris
{
    namespace io
    {
        Config readConfiguration(const std::string& filename)
        {
            std::ifstream infile(filename);

            if(!infile.is_open())
            {
                throw std::runtime_error("Could not read CFG file: " + filename);
            }

            auto config = parseConfigurationFromCFG(infile);
            infile.close();
            return config;
        }
        
        Config parseConfigurationFromCFG(std::istream& in)
        {
            typedef std::sregex_token_iterator tokenizer;
            typedef std::vector<std::string> tokens;

            Config config;
            std::string line;
            const std::regex pattern("[=]+");

            while(std::getline(in, line))
            {
                if(line.empty() || line.at(0) == '#' || line.at(line.size() - 1) == '\n')
                {
                    continue;
                }

                line = util::trim(line);
                tokens read = {tokenizer{line.begin(), line.end(), pattern, -1},
                               tokenizer{}};

                if(read.size() != 2)
                {
                    throw std::runtime_error("Parse error: number of tokens is"
                                             "not 2!");
                }

                // 1 - key, 1 - value
                config[util::trim(read[0])] = util::trim(read[1]);
            }
            
            return config;
        }
    }
}
