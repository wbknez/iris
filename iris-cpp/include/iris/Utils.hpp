#ifndef IRIS_UTILS_HPP_
#define IRIS_UTILS_HPP_

#include <algorithm>
#include <atomic>
#include <chrono>
#include <locale>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "iris/Types.hpp"

namespace iris
{
    namespace util
    {
        /*!
         * Performs a binary search (using the STL function <i>lower_bound</i>)
         * for the specified value on a container denoted by the specified
         * iterators.
         *
         * @param begin
         *        The start of the container to use.
         * @param end
         *        The end of the container to use.
         * @param val
         *        The value to search for.
         * @return An iterator to the specific location of the desired value in
         * the container, otherwise the end iterator.
         */
        template<class Iter, class T>
        inline Iter binarySearch(Iter begin, Iter end, T val)
        {
            auto found = std::lower_bound(begin, end, val);
            return ((found != end) && !(val < static_cast<T>((*found))))
                ? found : end;
        }

        /*!
         * Iterates over the lower half of the specified network, starting at
         * the specified index.
         *
         * Note that this function always returns a unique random number, but it
         * is up to the caller to ensure that it is within the proper bounds.
         *
         * @param r
         *        The random number to use (and check).
         * @param startIndex
         *        The starting index to use.
         * @param network
         *        The collection of previously chosen random numbers to check
         *        against, sorted in ascending order.
         * @return A unique random number.
         */
        template<typename T>
        inline T exploreLowerSphere(T r, T startIndex, const std::vector<T>& network)
        {
            for(T i = startIndex; i >= 0; i--)
            {
                if(i >= network.size())
                {
                    break;
                }
                
                if(r == network[i])
                {
                    --r;
                }
                else if(r > network[i])
                {
                    break;
                }
            }

            return r;
        }

        /*!
         * Iterates over the lower half of the specified network, starting at
         * the specified index.
         *
         * Note that this function always returns a unique random number, but it
         * is up to the caller to ensure that it is within the proper bounds.
         *
         * @param r
         *        The random number to use (and check).
         * @param startIndex
         *        The starting index to use.
         * @param network
         *        The collection of previously chosen random numbers to check
         *        against, sorted in ascending order.
         * @return A unique random number.
         */
        template<typename T>
        inline T exploreUpperSphere(T r, T startIndex, const std::vector<T>& network)
        {
            for(T i = startIndex; i < network.size(); i++)
            {
                if(r == network[i])
                {
                    ++r;
                }
                else if(r < network[i])
                {
                    break;
                }
            }

            return r;
        }

        /*!
         * Ensures that the specified random number <i>r</i> is unique (not
         * present) in the specified network and adheres to the specified
         * limits.
         *
         * In brief, this function searches the upper and lower portions of the
         * sample space, respectively, in order to find an interval therein that
         * may contain enough space for at least one more unique random number. 
         * If the specified network does not already contain the specified
         * random number then this is unnecessary and is in such a case skipped.
         *
         * It is critical to note that this function <b>requires</b> that the
         * network be sorted.  The core of the algorithm relies on the
         * self-sorting properties of a numerically ascending container, and so
         * this function is inappropriate for networks that have not been sorted
         * correctly.
         *
         * @param r
         *        The random number to use (and check).
         * @param network
         *        The collection of previously chosen random numbers to check
         *        against, sorted in ascending order.
         * @param lowerLimit
         *        The minimum value that a random number is allowed to be.
         * @param upperLimit
         *        The maximum value that a random number is allowed to be,
         *        given in standard bounds terms (so upperLimit - 1 is
         *        the true limiting value).
         * @return A unique random number.
         * @throws runtime_error
         *         If there is no random number available within the specified
         *         interval.
         */
        template<typename T>
        inline T ensureRandom(T r, const std::vector<T>& network,
                              T lowerLimit, T upperLimit)
        {
            // First, determine if r is present at all in the network.
            const auto startIter =
                binarySearch(network.begin(), network.end(), r);
            const auto startIndex =
                (typename std::vector<T>::size_type)
                (startIter - network.begin());

            if(startIndex >= network.size())
            {
                // No collision.
                return r;
            }

            // Otherwise, explore the upper portion of the sample space for an
            // improvement.
            auto rValue = exploreUpperSphere<T>(r, startIndex, network);

            // So, this is the tricky part.  If the program reaches these
            // statements, then we can infer that either:
            //    1) r is now greater than the largest value in the network but
            //    less than the upper limit, or
            //    2) not (1) because r violates the limit.
            //
            // The first case is trivial.
            if((rValue < network[network.size() - 1]) ||
                ((rValue > network[network.size() - 1]) && (rValue < upperLimit)))
            {
                return rValue;
            }

            // The second case is a repeat of the first.
            // Search the lower portion of the sample space, instead.
            rValue = exploreLowerSphere<T>(r, startIndex, network);

            // Same check as above, but with the lower bound.
            if(((rValue > network[0]) && (rValue < network[network.size() - 1]))
                || ((rValue < network[0]) && (rValue >= lowerLimit)))
            {
                return rValue;
            }

            // Otherwise, we ran out of space.
            throw std::runtime_error("There is no sample space left to use!");
        }
        
        /*!
         * Converts the specified string to a specific type.
         *
         * @param str
         *        The string to convert.
         * @return The string value as a different type.
         */
        template<typename T>
        inline T parseString(std::string str)
        {
            std::stringstream stream(str);
            T type;

            stream >> type;
            return type;
        }

        /*!
         * Inserts the specified element into the specified list in sorted
         * order, where "sorted" in this case means in ascending order.
         *
         * This function assumes the specified list is already in sorted order.
         *
         * @param container
         *        The sorted list to use.
         * @param val
         *        The element to insert.
         */
        template<class T>
        inline void sortedInsert(std::vector<T>& container, T val)
        {
            container.insert(std::lower_bound(container.begin(),
                                              container.end(),
                                              val),
                             val);
        }

        /*!
         * Repeatedly puts the current thread to sleep for the specified
         * amount of time while waiting for the stopping condition to be met.
         *
         * @param signal
         *        The current "signal" that may or may not be the stopping
         *        condition.
         * @param stopCondition
         *        The signal to wait for.
         * @param duration
         *        The length of time per sleep in milliseconds.
         */
        template<class T, class Rep, class Period>
        void spin(std::atomic<T>& signal, const T& stopCondition,
                  const std::chrono::duration<Rep, Period>& duration)
        {
            while(signal != stopCondition)
            {
                std::this_thread::sleep_for(duration);
            }
        }

        /*!
         * Repeatedly puts the current thread to sleep for the specified
         * amount of time while waiting for the stopping condition to be met
         * and while the specified invariant holds.
         *
         * This function is mostly used for testing.
         *
         * @param signal
         *        The current "signal" that may or may not be the stopping
         *        condition.
         * @param stopCondition
         *        The signal to wait for.
         * @param invariant
         *        A potentially new invariant, breaking the contract.
         * @param invariantCondition
         *        The invariant to maintain.
         * @param duration
         *        The length of time per sleep in milliseconds.
         */
        template<class T, class Rep, class Period>
        void spin(std::atomic<T>& signal, const T& stopCondition,
                  std::atomic<T>& invariant, const T& invariantCondition,
                  const std::chrono::duration<Rep, Period>& duration)
        {
            while((signal != stopCondition) &&
                  (invariant == invariantCondition))
            {
                std::this_thread::sleep_for(duration);
            }
        }

        /*!
         * Converts the specified typed value to a string.
         *
         * @param t
         *        The value to convert.
         * @return The value as a string.
         */
        template<typename T>
        inline constexpr std::string toString(T t)
        {
            std::stringstream stream;
            stream << t;
            return stream.str();
        }

        /*!
         * Removes any white-space from the left portion of the specified
         * string.
         *
         * @param str
         *        The string to trim (from the left).
         * @return A new, trimmed string.
         */
        inline std::string& ltrim(std::string& str)
        {
            auto iter = std::find_if(str.begin(), str.end(),
                [](char ch){
                    return !std::isspace<char>(ch, std::locale::classic());
                });
            str.erase(str.begin(), iter);
            return str;
        }

        /*!
         * Removes any white-space from the right portion of the specified
         * string.
         *
         * @param str
         *        The string to trim (from the right).
         * @return A new, trimmed string.
         */
        inline std::string& rtrim(std::string& str)
        {
            auto iter = std::find_if(str.rbegin(), str.rend(),
                [](char ch){
                    return !std::isspace<char>(ch, std::locale::classic());
                });
            str.erase(iter.base(), str.end());
            return str;
        }
        
        /*!
         * Removes white-space from the front and back of the specified string.
         *
         * @param str
         *        The string to trim.
         * @return The string with front and back white-space removed.
         */
        inline std::string trim(const std::string& str)
        {
            auto copy = str;
            return ltrim(rtrim(copy));
        }

        namespace term
        {
            /*!
             * A small collection of color codes (light).
             */
            enum Color
            {
                Black     = 30,
                Red       = 91,
                Green     = 92,
                Yellow    = 93,
                Blue      = 94,
                Magenta   = 95,
                Cyan      = 96,
                White     = 97,
                Default   = 39,
            };

            /*!
             * Represents a mechanism to write color codes to a stream and
             * thereby colorize all subsequent characters.
             */
            class Sequence
            {
                public:
                    /*!
                     * Constructor.
                     *
                     * @param color
                     *        The color code to use.
                     */
                    Sequence(Color color)
                        : m_color(color)
                    {}

                    /*!
                     * Writes the specified color sequence to the specified
                     * stream.
                     *
                     * @param out
                     *        The stream to write to (colorize).
                     * @para seq
                     *       The color sequence to write (use).
                     */
                    friend std::ostream& operator << (std::ostream& out,
                                                      const Sequence& seq)
                    {
                        return out << "\033[" << seq.m_color << "m";
                    }

                private:
                    /*!
                     * The color code for this sequence.
                     */
                    Color m_color;
            };
        }
    }
}

#endif
