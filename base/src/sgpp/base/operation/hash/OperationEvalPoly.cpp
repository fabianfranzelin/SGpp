// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at 
// sgpp.sparsegrids.org

#include <sgpp/base/operation/hash/OperationEvalPoly.hpp>

#include <sgpp/base/algorithm/GetAffectedBasisFunctions.hpp>
#include <sgpp/base/exception/operation_exception.hpp>

#include <sgpp/globaldef.hpp>


namespace SGPP {
  namespace base {

    float_t OperationEvalPoly::eval(DataVector& alpha, std::vector<float_t>& point) {
      typedef std::vector<std::pair<size_t, float_t> > IndexValVector;

      IndexValVector vec;
      GetAffectedBasisFunctions<SPolyBase> ga(storage);

      ga(base, point, vec);

      float_t result = 0.0;

      for (IndexValVector::iterator iter = vec.begin(); iter != vec.end(); iter++) {
        result += iter->second * alpha[iter->first];
      }

      return result;
    }

  }
}