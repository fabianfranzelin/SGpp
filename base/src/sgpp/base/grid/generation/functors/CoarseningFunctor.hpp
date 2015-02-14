// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at 
// sgpp.sparsegrids.org

#ifndef COARSENINGFUNCTOR_HPP
#define COARSENINGFUNCTOR_HPP

#include <sgpp/base/grid/GridStorage.hpp>

#include <sgpp/globaldef.hpp>


namespace SGPP {
  namespace base {

    /**
     * Abstract class that defines the interfaces that coarsening functors have to provide.
     * @version $HEAD$
     */
    class CoarseningFunctor {
      public:
        typedef float_t value_type;

        /**
         * Constructor
         */
        CoarseningFunctor() {}

        /**
         * Destructor
         */
        virtual ~CoarseningFunctor() {}

        /**
         * This should be returning a coarsening value for every grid point.
         * The point with the lowest value will be removed first.
         *
         * @param storage pointer to the grids storage object
         * @param seq sequence number in the coefficients array
         *
         * @return refinement value
         */
        virtual float_t operator()(GridStorage* storage, size_t seq) = 0;

        /**
         * This should return the initial value of coarsening criterion (e.g. alpha or error).
         *
         * @return the initial value
         */
        virtual float_t start() = 0;

        /**
         * Returns the maximal number of points that should be removed.
         *
         * The maximal number of points to removed is set in the constructor of implementation class.
         *
         * @return number of points that should removed. Default value: 1.
         */
        virtual size_t getRemovementsNum() {
          return 1;
        }

        /**
         * Returns the threshold value.
         *
         * Only the grid points with absolute value of coarsening criterion (e.g. alpha) less
         * or equal to this threshold will be removed
         *
         * @return threshold value for refinement. Default value: 0.
         */
        virtual float_t getCoarseningThreshold() = 0;
    };

  }
}

#endif /* COARSENINGFUNCTOR_HPP */