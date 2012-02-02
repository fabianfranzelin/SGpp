/******************************************************************************
* Copyright (C) 2012 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de)

#ifndef LEARNERVECTORIZED_HPP
#define LEARNERVECTORIZED_HPP

#include "datadriven/application/LearnerBase.hpp"

#include "parallel/tools/TypesParallel.hpp"

namespace sg
{

namespace parallel
{

/**
 * This class implements standard sparse grid regression
 * with an Identity matrix as regularization operator.
 *
 * Furthermore this Learner provides support for several
 * vectorization approaches covering GPUs, CPUs and coprocessors.
 */
class LearnerVectorized : public sg::datadriven::LearnerBase
{
protected:
	/// vectorization selector
	VectorizationType vecType_;

	virtual sg::datadriven::DMSystemMatrixBase* createDMSystem(sg::base::DataMatrix& trainDataset, double lambda);

	virtual void postProcessing(const sg::base::DataMatrix& trainDataset, const sg::solver::SLESolverType& solver,
			const size_t numNeededIterations);

public:
	/**
	 * Constructor
	 *
	 * @param vecType selection of vectorization to employ
	 * @param isRegression
	 * @param verbose
	 */
	LearnerVectorized(const VectorizationType vecType, const bool isRegression, const bool isVerbose = true);

	/**
	 * Constructor
	 *
	 * @param tGridFilename path to file that contains a serialized grid
	 * @param tAlphaFilenment path to file that contains the grid's coefficients
	 * @param vecType selection of vectorization to employ
	 * @param isRegression set to true if a regression task should be executed
	 * @param verbose set to true in order to allow console output
	 */
	LearnerVectorized(const std::string tGridFilename, const std::string tAlphaFilename, const VectorizationType vecType,
			const bool isRegression, const bool isVerbose = true);

	/**
	 * Destructor
	 */
	virtual ~LearnerVectorized();
};

}

}

#endif /* LEARNERVECTORIZED_HPP */
