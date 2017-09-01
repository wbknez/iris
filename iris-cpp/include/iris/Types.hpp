#ifndef IRIS_TYPES_HPP_
#define IRIS_TYPES_HPP_

#include <atomic>
#include <cstdint>
#include <random>
#include <utility>

namespace iris
{
    namespace types
    {
        typedef ::int8_t   int8;
        typedef ::uint8_t  uint8;

        typedef ::int32_t  int32;
        typedef ::uint32_t uint32;

        typedef ::int64_t  int64;
        typedef ::uint64_t uint64;

#if defined(IRIS_USE_64BIT_NUMERICS) || defined(IRIS_FORCE_64BIT_TYPES)
        typedef int64      numeric;
        typedef uint64     unumeric;
#else
        typedef int32      numeric;
        typedef uint32     unumeric;
#endif
        typedef double     fnumeric;

        typedef std::atomic<numeric>  atomic_numeric;
        typedef std::atomic<unumeric> atomic_unumeric;

        typedef std::mt19937          mersenne_twister;
    }

#if defined(IRIS_USE_64BIT_IDS) || defined(IRIS_FORCE_64BIT_TYPES)
    typedef types::uint64  AgentID;
#else
    typedef types::uint32  AgentID;
#endif

    typedef std::pair<AgentID, AgentID>   IDInterval;

    /*
     * Behavior/value types.
     *
     * For purposes of this simulation, the relationship between values and
     * behaviors as spoken of in e.g. Hawkes (1975) is bijective.  This prevents
     * conceptual difficulties when discussing the potential relationship
     * between the two, since this project is not focused on a hyper-realistic
     * model of normative behavior.  Indeed, it is sufficient to have a suitably
     * random mechanism for behavior transmission in order to underscore how
     * graph topology and in-group composition may affect individual outcomes.
     *
     * Behavior/value lists are read in as input from a CSV file.
     */
    typedef std::vector<types::uint32>    Uint32List;
    typedef Uint32List                    BehaviorList;
    typedef Uint32List                    ValueList;
}

#endif
