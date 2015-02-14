// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#ifndef RANDOM_HPP
#define RANDOM_HPP

#ifdef USETRONE
#define USE_STD_RANDOM 1
#endif

#ifdef USE_STD_RANDOM
#include <random>
#endif

#include <sgpp/globaldef.hpp>


namespace SGPP {
  namespace quadrature {

    /**
     * Wraps the random generator to use. Ensures that it is is seeded correctly.
     */

    class Random {
      public:

        /**
         * Manually seed the generator with a given seed.
         * @param seed_value the seed to use.
         */
        static void seed(int seed_value);

        /**
         * returns a random integer value.
         * @see rand()
         */
        static int random();

        /**
         * returns a random float_t value,
         * like rand()/RAND_MAX
         * @see rand()
         */
        static float_t random_double();

      protected:
        // whether the RNG has aleredy been initialized.
        static bool is_seeded;

#ifdef USE_STD_RANDOM
        static std::mt19937 gen;
        static std::uniform_int_distribution<int> dist;
#endif
    };
  }
}

#endif /* RANDOM_HPP */