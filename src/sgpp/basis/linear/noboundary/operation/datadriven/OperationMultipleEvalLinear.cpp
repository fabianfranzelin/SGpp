/******************************************************************************
* Copyright (C) 2009 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Jörg Blank (blankj@in.tum.de), Alexander Heinecke (Alexander.Heinecke@mytum.de)

#include "sgpp.hpp"
#include "basis/basis.hpp"
#include "basis/linear/noboundary/operation/datadriven/OperationMultipleEvalLinear.hpp"
#include "exception/operation_exception.hpp"
#include "data/DataVector.hpp"
#include "data/DataMatrix.hpp"


namespace sg
{
namespace base
{

void OperationMultipleEvalLinear::mult(DataVector& alpha, DataVector& result)
{
	AlgorithmMultipleEvaluation<SLinearBase> op;
	linear_base<unsigned int, unsigned int> base;

	op.mult(storage, base, alpha, *(this->dataset_), result);
}

void OperationMultipleEvalLinear::multTranspose(DataVector& alpha, DataVector& result)
{
	AlgorithmMultipleEvaluation<SLinearBase> op;
	linear_base<unsigned int, unsigned int> base;

	op.mult_transpose(storage, base, alpha, *(this->dataset_), result);
}

}
}
