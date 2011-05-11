/******************************************************************************
* Copyright (C) 2009 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Chao qi(qic@in.tum.de)

#include "basis/linear/noboundary/operation/pde/financeHW1D/OperationLFLinear.hpp"

#include "basis/linear/noboundary/algorithm_sweep/XdPhiPhiDownBBLinear.hpp"
#include "basis/linear/noboundary/algorithm_sweep/XdPhiPhiUpBBLinear.hpp"

#include "algorithm/common/sweep.hpp"
using namespace sg::pde;
using namespace sg::base;

namespace sg
{
namespace finance
{

OperationLFLinear::OperationLFLinear(GridStorage* storage) : StdUpDown(storage)
{
}

OperationLFLinear::~OperationLFLinear()
{
}

void OperationLFLinear::up(DataVector& alpha, DataVector& result, size_t dim)
{
	// X * dphi * phi
	XdPhiPhiUpBBLinear func(this->storage);
	sweep<XdPhiPhiUpBBLinear> s(func, this->storage);

	s.sweep1D(alpha, result, dim);
}

void OperationLFLinear::down(DataVector& alpha, DataVector& result, size_t dim)
{
	// X * dphi * phi
	XdPhiPhiDownBBLinear func(this->storage);
	sweep<XdPhiPhiDownBBLinear> s(func, this->storage);

	s.sweep1D(alpha, result, dim);
}

}
}
