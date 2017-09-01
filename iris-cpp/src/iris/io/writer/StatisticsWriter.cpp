#include "iris/io/writer/StatisticsWriter.hpp"

#include <ctime>
#include <locale>
#include <stdexcept>
#include <utility>

#include <sys/stat.h>

#include "iris/Agent.hpp"
#include "iris/Utils.hpp"

namespace iris
{
    namespace io
    {
        std::string getDateAndTime()
        {
            std::locale::global(std::locale("en_US.utf8"));
            std::time_t t = std::time(NULL);
            char tStr[80];

            std::strftime(tStr, sizeof(tStr), "%y%b%d-%H%M%S", std::localtime(&t));
            return std::string(tStr);
        }

        std::string createDataDirectory(const std::string& where,
                                        types::uint32 run)
        {            
            // Obtain the date and time for use as a directory name.
            const auto dirName = "run-" +  getDateAndTime()+
                                 "-" + util::toString(run);

            // Combine the directory and top level path.
            const auto dirPath = where + "/" + dirName;

            if(mkdir(dirPath.c_str(), S_IRWXU | S_IRWXG))
            {
                throw std::runtime_error("Could not create directory " +
                                         dirPath);
            }

            return dirPath;
        }

        StatisticsWriter::StatisticsWriter()
        {}

        StatisticsWriter::~StatisticsWriter()
        {}

        void StatisticsWriter::clear()
        {
            using namespace iris::types;
            
            for(auto& pe : m_permutes)
            {
                m_census[pe] = static_cast<uint32>(0);
            }
        }

        void StatisticsWriter::initialize(const Uint32List &behavior)
        {
            using namespace iris::gen;
            using namespace iris::types;

            m_permutes = permuteList(behavior);

            for(auto& perm : m_permutes)
            {
                const auto defValue = static_cast<uint32>(0);
                const auto cenValue =
                    std::pair<std::string, uint32>(perm, defValue);
                
                m_census.insert(cenValue);
            }
        }

        void StatisticsWriter::writeHeader(std::ostream &out)
        {
            using namespace iris::gen;            
            out << "Time,Privilege,";

            for(PermuteList::size_type i = 0; i < m_permutes.size(); i++)
            {
                out << m_permutes[i];
                
                if(i < (m_permutes.size() - 1))
                {
                    out << ",";
                }
            }

            out << std::endl;
        }
        
        void StatisticsWriter::writeStatistics(std::ostream &out,
                                               Agent *const agents,
                                               AgentID totalAgents,
                                               types::uint64 currentTime)
        {
            using namespace iris::gen;
            using namespace iris::types;
            
            this->clear();
            uint64 totalPrivilege = 0;
            
            for(AgentID i = 0; i < totalAgents; i++)
            {
                const auto key = convertListToString(agents[i].getBehavior());
                m_census[key] += 1;

                totalPrivilege += agents[i].getPrivilege();                
            }

            out << currentTime << "," << totalPrivilege << ",";

            for(PermuteList::size_type j = 0; j < m_permutes.size(); j++)
            {
                out << m_census[m_permutes[j]];

                if(j < (m_permutes.size() - 1))
                {
                    out << ",";
                }
            }

            out << std::endl;
        }
    }
}
