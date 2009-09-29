/*****************************************************************************/
/* This file is part of sgpp, a program package making use of spatially      */
/* adaptive sparse grids to solve numerical problems                         */
/*                                                                           */
/* Copyright (C) 2009 Alexander Heinecke (Alexander.Heinecke@mytum.de)       */
/*                                                                           */
/* sgpp is free software; you can redistribute it and/or modify              */
/* it under the terms of the GNU Lesser General Public License as published  */
/* by the Free Software Foundation; either version 3 of the License, or      */
/* (at your option) any later version.                                       */
/*                                                                           */
/* sgpp is distributed in the hope that it will be useful,                   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU Lesser General Public License for more details.                       */
/*                                                                           */
/* You should have received a copy of the GNU Lesser General Public License  */
/* along with sgpp; if not, write to the Free Software                       */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/* or see <http://www.gnu.org/licenses/>.                                    */
/*****************************************************************************/

#include "basis/lineartrapezoidboundary/operation/pde/finance/OperationGammaPartTwoLinearTrapezoidBoundary.hpp"

#include "basis/lineartrapezoidboundary/algorithm_sweep/PhiPhiDownBBLinearTrapezoidBoundary.hpp"
#include "basis/lineartrapezoidboundary/algorithm_sweep/PhiPhiUpBBLinearTrapezoidBoundary.hpp"

#include "basis/lineartrapezoidboundary/algorithm_sweep/XdPhiPhiDownBBLinearTrapezoidBoundary.hpp"
#include "basis/lineartrapezoidboundary/algorithm_sweep/XdPhiPhiUpBBLinearTrapezoidBoundary.hpp"

#include "algorithm/common/sweep.hpp"

#include <iostream>

namespace sg
{

OperationGammaPartTwoLinearTrapezoidBoundary::OperationGammaPartTwoLinearTrapezoidBoundary(GridStorage* storage, DataVector& sigma, DataVector& rho)
{
	this->storage = storage;
	this->sigmas = &sigma;
	this->rhos = &rho;
}

OperationGammaPartTwoLinearTrapezoidBoundary::~OperationGammaPartTwoLinearTrapezoidBoundary()
{
}

void OperationGammaPartTwoLinearTrapezoidBoundary::mult(DataVector& alpha, DataVector& result)
{
	result.setAll(0.0);
#ifdef USEOMP
#ifdef USEOMPTHREE
	DataVector beta(result.getSize());

	for(size_t i = 0; i < storage->dim(); i++)
	{
		#pragma omp parallel
		{
			#pragma omp single nowait
			{
				this->updown_parallel(alpha, beta, storage->dim() - 1, i);
			}
		}
		for(size_t j = 0; j < storage->dim(); j++)
		{
			if (i == j)
			{
				result.axpy(sigmas->get(i)*sigmas->get(j)*rhos->get((storage->dim()*i)+j),beta);
			}
			else
			{
				result.axpy((0.5)*sigmas->get(i)*sigmas->get(j)*rhos->get((storage->dim()*i)+j),beta);
			}
		}
	}
#endif
#ifndef USEOMPTHREE
	#pragma omp parallel shared(result)
	{
		#pragma omp for schedule(static)
		for(size_t i = 0; i < storage->dim(); i++)
		{
			DataVector beta(result.getSize());
			this->updown(alpha, beta, storage->dim() - 1, i);

			for(size_t j = 0; j < storage->dim(); j++)
			{
				if (i == j)
				{
					#pragma omp critical
					result.axpy(sigmas->get(i)*sigmas->get(j)*rhos->get((storage->dim()*i)+j),beta);
				}
				else
				{
					#pragma omp critical
					result.axpy((0.5)*sigmas->get(i)*sigmas->get(j)*rhos->get((storage->dim()*i)+j),beta);
				}
			}
		}
	}
#endif
#endif
#ifndef USEOMP
	DataVector beta(result.getSize());

	for(size_t i = 0; i < storage->dim(); i++)
	{
		this->updown(alpha, beta, storage->dim() - 1, i);
		for(size_t j = 0; j < storage->dim(); j++)
		{
			if (i == j)
			{
				result.axpy(sigmas->get(i)*sigmas->get(j)*rhos->get((storage->dim()*i)+j),beta);
			}
			else
			{
				result.axpy((0.5)*sigmas->get(i)*sigmas->get(j)*rhos->get((storage->dim()*i)+j),beta);
			}
		}
	}
#endif
}

#ifndef USEOMPTHREE
void OperationGammaPartTwoLinearTrapezoidBoundary::updown(DataVector& alpha, DataVector& result, size_t dim, size_t gradient_dim)
{
	if(dim == gradient_dim)
	{
		gradient(alpha, result, dim, gradient_dim);
	}
	else
	{
		//Unidirectional scheme
		if(dim > 0)
		{
			// Reordering ups and downs
			DataVector temp(alpha.getSize());
			up(alpha, temp, dim);
			updown(temp, result, dim-1, gradient_dim);


			// Same from the other direction:
			DataVector result_temp(alpha.getSize());
			updown(alpha, temp, dim-1, gradient_dim);
			down(temp, result_temp, dim);

			result.add(result_temp);
		}
		else
		{
			// Terminates dimension recursion
			up(alpha, result, dim);

			DataVector temp(alpha.getSize());
			down(alpha, temp, dim);

			result.add(temp);
		}
	}
}

void OperationGammaPartTwoLinearTrapezoidBoundary::gradient(DataVector& alpha, DataVector& result, size_t dim, size_t gradient_dim)
{
	//Unidirectional scheme
	if(dim > 0)
	{
		// Reordering ups and downs
		DataVector temp(alpha.getSize());
		upGradient(alpha, temp, dim);
		updown(temp, result, dim-1, gradient_dim);


		// Same from the other direction:
		DataVector result_temp(alpha.getSize());
		updown(alpha, temp, dim-1, gradient_dim);
		downGradient(temp, result_temp, dim);

		result.add(result_temp);
	}
	else
	{
		// Terminates dimension recursion
		upGradient(alpha, result, dim);

		DataVector temp(alpha.getSize());
		downGradient(alpha, temp, dim);

		result.add(temp);
	}
}
#endif

#ifdef USEOMPTHREE
void OperationGammaPartTwoLinearTrapezoidBoundary::updown_parallel(DataVector& alpha, DataVector& result, size_t dim, size_t gradient_dim)
{
	if(dim == gradient_dim)
	{
		gradient_parallel(alpha, result, dim, gradient_dim);
	}
	else
	{
		//Unidirectional scheme
		if(dim > 0)
		{
			// Reordering ups and downs
			DataVector temp(alpha.getSize());
			DataVector result_temp(alpha.getSize());
			DataVector temp_two(alpha.getSize());

			#pragma omp task shared(alpha, temp, result)
			{
				up(alpha, temp, dim);
				updown_parallel(temp, result, dim-1, gradient_dim);
			}

			// Same from the other direction:
			#pragma omp task shared(alpha, temp_two, result_temp)
			{
				updown_parallel(alpha, temp_two, dim-1, gradient_dim);
				down(temp_two, result_temp, dim);
			}

			#pragma omp taskwait

			result.add(result_temp);
		}
		else
		{
			// Terminates dimension recursion
			DataVector temp(alpha.getSize());

			#pragma omp task shared(alpha, result)
			up(alpha, result, dim);

			#pragma omp task shared(alpha, temp)
			down(alpha, temp, dim);

			#pragma omp taskwait

			result.add(temp);
		}
	}
}

void OperationGammaPartTwoLinearTrapezoidBoundary::gradient_parallel(DataVector& alpha, DataVector& result, size_t dim, size_t gradient_dim)
{
	//Unidirectional scheme
	if(dim > 0)
	{
		// Reordering ups and downs
		DataVector temp(alpha.getSize());
		DataVector result_temp(alpha.getSize());
		DataVector temp_two(alpha.getSize());

		#pragma omp task shared(alpha, temp, result)
		{
			upGradient(alpha, temp, dim);
			updown_parallel(temp, result, dim-1, gradient_dim);
		}

		// Same from the other direction:
		#pragma omp task shared(alpha, temp_two, result_temp)
		{
			updown_parallel(alpha, temp_two, dim-1, gradient_dim);
			downGradient(temp_two, result_temp, dim);
		}

		#pragma omp taskwait

		result.add(result_temp);
	}
	else
	{
		// Terminates dimension recursion
		DataVector temp(alpha.getSize());

		#pragma omp task shared(alpha, result)
		upGradient(alpha, result, dim);

		#pragma omp task shared(alpha, temp)
		downGradient(alpha, temp, dim);

		#pragma omp taskwait

		result.add(temp);
	}
}
#endif

void OperationGammaPartTwoLinearTrapezoidBoundary::up(DataVector& alpha, DataVector& result, size_t dim)
{
	// phi * phi
	detail::PhiPhiUpBBLinearTrapezoidBoundary func(this->storage);
	sweep<detail::PhiPhiUpBBLinearTrapezoidBoundary> s(func, this->storage);

	s.sweep1D_Boundary(alpha, result, dim);
}

void OperationGammaPartTwoLinearTrapezoidBoundary::down(DataVector& alpha, DataVector& result, size_t dim)
{
	// phi * phi
	detail::PhiPhiDownBBLinearTrapezoidBoundary func(this->storage);
	sweep<detail::PhiPhiDownBBLinearTrapezoidBoundary> s(func, this->storage);

	s.sweep1D_Boundary(alpha, result, dim);
}

void OperationGammaPartTwoLinearTrapezoidBoundary::upGradient(DataVector& alpha, DataVector& result, size_t dim)
{
	// x * dphi * phi
	detail::XdPhiPhiUpBBLinearTrapezoidBoundary func(this->storage);
	sweep<detail::XdPhiPhiUpBBLinearTrapezoidBoundary> s(func, this->storage);

	s.sweep1D_Boundary(alpha, result, dim);
}

void OperationGammaPartTwoLinearTrapezoidBoundary::downGradient(DataVector& alpha, DataVector& result, size_t dim)
{
	// x * dphi * phi
	detail::XdPhiPhiDownBBLinearTrapezoidBoundary func(this->storage);
	sweep<detail::XdPhiPhiDownBBLinearTrapezoidBoundary> s(func, this->storage);

	s.sweep1D_Boundary(alpha, result, dim);
}

}
