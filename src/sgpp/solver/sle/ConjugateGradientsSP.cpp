/******************************************************************************
* Copyright (C) 2010 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de)

#include "solver/sle/ConjugateGradientsSP.hpp"

namespace sg
{

ConjugateGradientsSP::ConjugateGradientsSP(size_t imax, float epsilon) : nMaxIterations(imax), myEpsilon(epsilon)
{
}

ConjugateGradientsSP::~ConjugateGradientsSP()
{
}

void ConjugateGradientsSP::solve(OperationMatrixSP& SystemMatrix, DataVectorSP& alpha, DataVectorSP& b, bool reuse, bool verbose, float max_threshold)
{
	if (verbose == true)
	{
		std::cout << "Starting Conjugated Gradients" << std::endl;
	}

	// needed for residuum calculation
	float epsilonSquared = this->myEpsilon*this->myEpsilon;
	// number off current iterations
	this->nIterations = 0;

	// define temporal vectors
	DataVectorSP temp(alpha.getSize());
	DataVectorSP q(alpha.getSize());
	DataVectorSP r(b);

	float delta_0 = 0.0;
	float delta_old = 0.0;
	float delta_new = 0.0;
	float beta = 0.0;
	float a = 0.0;

	if (verbose == true)
	{
		std::cout << "All temp variables used in CG have been initialized" << std::endl;
	}

	if (reuse == true)
	{
		q.setAll(0.0);
		SystemMatrix.mult(q, temp);
		r.sub(temp);
		delta_0 = r.dotProduct(r)*epsilonSquared;
	}
	else
	{
		alpha.setAll(0.0);
	}

	// calculate the starting residuum
	SystemMatrix.mult(alpha, temp);
	r.sub(temp);

	DataVectorSP d(r);

	delta_old = 0.0;
	delta_new = r.dotProduct(r);

	if (reuse == false)
	{
		delta_0 = delta_new*epsilonSquared;
	}

	this->residuum = (delta_0/epsilonSquared);

	if (verbose == true)
	{
		std::cout << "Starting norm of residuum: " << (delta_0/epsilonSquared) << std::endl;
		std::cout << "Target norm:               " << (delta_0) << std::endl;
	}

	while ((this->nIterations < this->nMaxIterations) && (delta_new > delta_0) && (delta_new > max_threshold))
	{
		// q = A*d
		SystemMatrix.mult(d, q);

		// a = d_new / d.q
		a = delta_new/d.dotProduct(q);

		// x = x + a*d
		alpha.axpy(a, d);

		// Why ????
		if ((this->nIterations % 50) == 0)
		{
			// r = b - A*x
			SystemMatrix.mult(alpha, temp);
			r.copyFrom(b);
			r.sub(temp);
		}
		else
		{
			// r = r - a*q
			r.axpy(-a, q);
		}


		// calculate new deltas and determine beta
		delta_old = delta_new;
		delta_new = r.dotProduct(r);
		beta = delta_new/delta_old;

		this->residuum = delta_new;

		if (verbose == true)
		{
			std::cout << "delta: " << delta_new << std::endl;
		}

		d.mult(beta);
		d.add(r);

		this->nIterations++;
	}

	this->residuum = delta_new;

	if (verbose == true)
	{
		std::cout << "Number of iterations: " << this->nIterations << " (max. " << this->nMaxIterations << ")" << std::endl;
		std::cout << "Final norm of residuum: " << delta_new << std::endl;
	}
}

size_t ConjugateGradientsSP::getNumberIterations()
{
	return nIterations;
}

float ConjugateGradientsSP::getResiduum()
{
	return residuum;
}

void ConjugateGradientsSP::setMaxIterations(size_t nIterations)
{
	nMaxIterations = nIterations;
}

void ConjugateGradientsSP::setEpsilon(float eps)
{
	myEpsilon = eps;
}

float ConjugateGradientsSP::getEpsilon()
{
	return myEpsilon;
}

}
