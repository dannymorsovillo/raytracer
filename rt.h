#ifndef RT_H_
#define RT_H_

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <thread>       // for std::this_thread::get_id
#include <functional>   // for std::hash
#include <cstdint>      // for uint64_t
#include <cstdlib>      // (kept for compatibility if you still use std::rand elsewhere)

using std::make_shared;
using std::shared_ptr;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

// ----------------------
// Thread-safe RNG
// ----------------------

// Per-thread generator (seeded from random_device XOR thread id)
inline std::mt19937_64& rng() {
    thread_local std::mt19937_64 gen(
        std::random_device{}() ^
        (uint64_t)std::hash<std::thread::id>{}(std::this_thread::get_id())
    );
    return gen;
}

// Uniform [0,1)
inline double random_double() {
    thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng());
}

// Uniform [min,max)
inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

// Optional: integer in [min,max]
inline int random_int(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng());
}

#include "color.h"
#include "ray.h"
#include "vec_3.h"

#endif // RT_H_
