#include "iris/io/CommandLine.hpp"

#include <iostream>
#include <utility>

#include "iris/Utils.hpp"

namespace iris
{
    namespace io
    {
        Options::Options()
        {}

        Options::~Options()
        {
            m_commands.clear();
        }

        bool Options::has(const std::string& name) const
        {
            return m_commands.find(name) != m_commands.end();
        }

        bool Options::has(const std::string& name, types::uint32 numArgs) const
        {
            const auto found = m_commands.find(name);
            return found != m_commands.end() && found->second.size() == numArgs;
        }

        bool Options::hasSingle(const std::string& name) const
        {
            const auto found = m_commands.find(name);
            return found != m_commands.end() && found->second.size() == 1;
        }

        bool Options::hasMultiple(const std::string& name) const
        {
            const auto found = m_commands.find(name);
            return found != m_commands.end() && found->second.size() > 1;
        }

        void Options::set(std::string name)
        {
            Arguments empty;
            this->set(name, empty);
        }
        
        void Options::set(std::string name, std::string arg)
        {
            Arguments argList{arg};
            this->set(name, argList);
        }

        void Options::set(std::string name, Arguments args)
        {
            const auto command = std::pair<std::string, Arguments>(name, args);
            m_commands.insert(command);
        }

        CommandParser::CommandParser()
        {}

        CommandParser::~CommandParser()
        {}

        void CommandParser::addOption(std::string name, std::string desc)
        {
            this->addOption(name, 0, desc);
        }
        
        void CommandParser::addOption(std::string name, types::uint32 numArgs,
                                      std::string desc)
        {
            const auto argSpec =
                std::pair<std::string, types::uint32>(name, numArgs);
            const auto descSpec =
                std::pair<std::string, std::string>(name, desc);

            if(!m_args.insert(argSpec).second)
            {
                throw std::runtime_error("Could not add argument specification"
                                         " for: " + name);
            }

            if(!m_desc.insert(descSpec).second)
            {
                throw std::runtime_error("Could not add desc specification"
                                         " for: " + name);
            }
        }

        CommandParser::Tokens CommandParser::convertToTokens(int argc,
                                                             char **argv)
        {
            Tokens tokens;
            
            for(auto i = 0; i < argc; i++)
            {
                tokens.push_back(argv[i]);
            }

            return tokens;
        }

        Options::Arguments CommandParser::extractArguments(types::uint32 start,
                                                           types::uint32 end,
                                                   CommandParser::Tokens tokens)
        {
            // The list of collected arguments (converted to strings).
            Options::Arguments collected;

            for(auto i = start; i < end; i++)
            {
                collected.push_back(tokens[i]);
            }

            return collected;
        }
        
        types::uint32 CommandParser::findNextOption(types::uint32 currentIndex,
                                            const CommandParser::Tokens& tokens)
        {
            // The pattern to match against (long-form only).
            const std::string pattern("--");

            for(auto i = currentIndex + 1; i < tokens.size(); i++)
            {
                // Check against the pattern.
                if(tokens[i].compare(0, pattern.length(), pattern) == 0)
                {
                    return i;
                }
            }

            return tokens.size();
        }
        
        Options CommandParser::parse(types::uint32 argc, char** argv)
        {
            const auto tokens = this->convertToTokens(argc, argv);
            return this->parse(tokens);
        }
      
        Options CommandParser::parse(CommandParser::Tokens tokens)
        {
            // The collected options.
            Options options;
            
            // The current index.
            types::uint32 currentIndex = this->findNextOption(0, tokens);

            while(currentIndex < tokens.size())
            {
                // Find the "next" location of an option.
                const auto nextIndex =
                    this->findNextOption(currentIndex, tokens);
                
                // Grab the "current" option in its entirety.
                std::string currentOption(tokens[currentIndex]);

                // Fix by removing the long-form pattern.
                currentOption = currentOption.substr(2, currentOption.size());
                
                // Figure out that the arguments are.
                Options::Arguments argList =
                    this->extractArguments(currentIndex + 1, nextIndex, tokens);

                // Finally, determine if a specification exists.
                const auto found = m_args.find(currentOption);

                if(found == m_args.end())
                {
                    throw std::runtime_error("No specification found for:" +
                                             currentOption);
                }
                
                if(found->second != argList.size())
                {
                    throw std::runtime_error("Mismatch in expected arguments"
                        " for: " + currentOption + " "
                        "Found: " + util::toString(argList.size()) +
                        " Expected: " + util::toString(found->second));
                }

                // Save and move forward.
                options.set(currentOption, argList);
                currentIndex = nextIndex;
            }

            return options;
        }

        void CommandParser::printUsage(const std::string& progName)
        {
            std::cout << progName << " [OPTIONS]" << std::endl;
            std::cout << std::endl;

            for(auto entry : m_desc)
            {
                std::cout << entry.first << "\t" << entry.second << std::endl;
            }
        }

        void CommandParser::removeOption(const std::string &name)
        {
            m_args.erase(name);
            m_desc.erase(name);
        }
    }
}
