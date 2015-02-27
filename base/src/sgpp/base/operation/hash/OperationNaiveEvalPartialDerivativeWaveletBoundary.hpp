// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#ifndef OPERATIONNAIVEEVALPARTIALDERIVATIVEWAVELETBOUNDARY_HPP
#define OPERATIONNAIVEEVALPARTIALDERIVATIVEWAVELETBOUNDARY_HPP

#include <sgpp/globaldef.hpp>
#include <sgpp/base/operation/hash/OperationNaiveEvalPartialDerivative.hpp>
#include <sgpp/base/grid/GridStorage.hpp>
#include <sgpp/base/operation/hash/common/basis/WaveletBoundaryBasis.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>

namespace SGPP {
  namespace base {

    /**
     * Operation for evaluating partial derivatives of wavelet
     * linear combinations on Boundary grids.
     */
    class OperationNaiveEvalPartialDerivativeWaveletBoundary :
      public OperationNaiveEvalPartialDerivative {
      public:
        /**
         * Constructor.
         *
         * @param storage   storage of the sparse grid
         */
        OperationNaiveEvalPartialDerivativeWaveletBoundary(GridStorage* storage) :
          storage(storage) {
        }

        /**
         * Virtual destructor.
         */
        virtual ~OperationNaiveEvalPartialDerivativeWaveletBoundary() {
        }

        /**
         * @param alpha     coefficient vector
         * @param point     evaluation point
         * @param deriv_dim dimension in which the partial derivative should be taken
         * @return          value of the partial derivative of the linear combination
         */
        virtual float_t evalPartialDerivative(DataVector& alpha,
                                              const std::vector<float_t>& point,
                                              size_t deriv_dim);

      protected:
        /// storage of the sparse grid
        GridStorage* storage;
        /// 1D wavelet basis
        SWaveletBoundaryBase base;
    };

  }
}

#endif /* OPERATIONNAIVEEVALPARTIALDERIVATIVEWAVELETBOUNDARY_HPP */