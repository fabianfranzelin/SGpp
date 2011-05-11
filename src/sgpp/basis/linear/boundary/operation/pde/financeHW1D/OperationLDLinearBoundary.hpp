/******************************************************************************
* Copyright (C) 2009 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Chao qi(qic@in.tum.de)

#ifndef OPERATIONLDLINEARBOUNDARY_HPP
#define OPERATIONLDLINEARBOUNDARY_HPP

#include "algorithm/pde/StdUpDown.hpp"
using namespace sg::pde;
using namespace sg::base;

namespace sg
{
namespace finance
{

/**
 * Implements the \f$(x \phi_i(x),\phi_j(x))\f$ operator on linear boundary grids
 *
 * @version $HEAD$
 */
class OperationLDLinearBoundary: public StdUpDown
{
public:
	/**
	 * Constructor
	 *
	 * @param storage the grid's GridStorage object
	 */
	OperationLDLinearBoundary(GridStorage* storage);

	/**
	 * Destructor
	 */
	virtual ~OperationLDLinearBoundary();

protected:
	/**
	 * Up-step in dimension <i>dim</i> for \f$(x \phi_i(x),\phi_j(x))\f$.
	 * Applies the up-part of the one-dimensional mass matrix in one dimension.
	 * Computes \f[\int_{x=0}^1  x \phi_i(x) \phi_j(x) dx.\f]
	 *
	 * @param dim dimension in which to apply the up-part
	 * @param alpha vector of coefficients
	 * @param result vector to store the results in
	 */
	virtual void up(DataVector& alpha, DataVector& result, size_t dim);

	/**
	 * Down-step in dimension <i>dim</i> for \f$(x \phi_i(x),\phi_j(x))\f$.
	 * Applies the down-part of the one-dimensional mass matrix in one dimension.
	 * Computes \f[\int_{x=0}^1  x \phi_i(x) \phi_j(x) dx.\f]
	 *
	 * @param dim dimension in which to apply the down-part
	 * @param alpha vector of coefficients
	 * @param result vector to store the results in
	 */
	virtual void down(DataVector& alpha, DataVector& result, size_t dim);
};

}
}

#endif /* OPERATIONLDLINEARBOUNDARY_HPP */
