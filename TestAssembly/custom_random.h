#ifndef ANG_TOOLS_RANDOM_H
#define ANG_TOOLS_RANDOM_H
#include <cstdint>
#include <limits>
#include <random>
#include "Macros.h"

/// \see http://www.pcg-random.org/posts/developing-a-seed_seq-alternative.html
/// as it inspired this whole namespace
namespace ANG::tools::random {

/// thread safe access to actual pseudo randomness aka the generator
inline std::uint64_t next()
{
    /// don't use \see std::random_device as it's bad implemented, slow and its source is non predictable
    /// \see http://www.pcg-random.org/posts/cpp-seeding-surprises.html
    unsigned long long val;
    // gcc intrinsic which requires -mrdrnd see https://en.wikipedia.org/wiki/RdRand
    __builtin_ia32_rdrand64_step(&val); // the smaller ones just deliver less bits for the same amount of processing
    return val;
}

/// cheap stateless throw away wrapper for \section Random number distributions \see https://en.cppreference.com/w/cpp/numeric/random
template<typename T=std::uint64_t> class Generator
{
public:
    using result_type = T;

    static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
    static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

    result_type operator()() const
    {
        if constexpr (sizeof(result_type) == sizeof(std::uint64_t))
            return result_type(next());
        else
        {
            // only extract as few bytes as needed
            thread_local std::uint64_t data;
            thread_local std::int_fast8_t remainingByte = 0;
            if (remainingByte == 0)
            {
                data = next();
                remainingByte = sizeof(data);
            }
            result_type t = data;
            data >>= sizeof(result_type)*8;
            remainingByte -= sizeof(result_type);
            return t;
        }
    }
};

/// \see std::uniform_int/real_distribution can be created and discarded on demand
///     in that case they might waste some random bits but it otherwise harmless i.e. not wasting too much resources
/// \attention also don't use this function for all other \see std::something_distribution as they need to stay alive to achieve their specific result
/// \param min and max are inclusive
/// \param T should be as small as possible to cover \param min and max but not too small,
///     but even if it would the distribution would just issue more requests for randomness
template<typename T=int> T nextUniformDistributionValue(T min, T max)
{
    std::uniform_int_distribution<T> dist(min, max);
    Generator<std::make_unsigned_t<T>> gen;
    return dist(gen);
}

inline bool flipCoin()
{
    thread_local std::uint64_t data;
    thread_local std::int_fast8_t remainingBits = 0;
    if (remainingBits == 0)
    {
        data = next();
        remainingBits = sizeof(data);
    }
    bool b = data & 0x01;
    data >>= 1;
    remainingBits--;
    return b;
}

/// \returns true if in favor of \param weight
/// e.g. will \return true in about 80% of the cases if \param weight is 0.8
inline bool flipWeightedCoin(double weight)
{
    ASSERT(0 < weight && weight < 1);
    std::discrete_distribution<Generator<>::result_type> dist({1-weight, weight});
    Generator<> gen;
    return dist(gen);
}

}

#endif
