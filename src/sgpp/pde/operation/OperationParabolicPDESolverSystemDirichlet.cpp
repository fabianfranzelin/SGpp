/******************************************************************************
* Copyright (C) 2010 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de)

#include "pde/operation/OperationParabolicPDESolverSystemDirichlet.hpp"
#include "base/exception/algorithm_exception.hpp"

namespace sg
{
namespace pde
{

OperationParabolicPDESolverSystemDirichlet::OperationParabolicPDESolverSystemDirichlet()
{
	this->numSumGridpointsInner = 0;
	this->numSumGridpointsComplete = 0;
}

OperationParabolicPDESolverSystemDirichlet::~OperationParabolicPDESolverSystemDirichlet()
{
}

void OperationParabolicPDESolverSystemDirichlet::mult(sg::base::DataVector& alpha, sg::base::DataVector& result)
{
	if (this->tOperationMode == "ExEul")
	{
		applyMassMatrixInner(alpha, result);
	}
	else if (this->tOperationMode == "ImEul")
	{
		result.setAll(0.0);

		sg::base::DataVector temp(alpha.getSize());

		applyMassMatrixInner(alpha, temp);
		result.add(temp);

		applyLOperatorInner(alpha, temp);
		result.axpy((-1.0)*this->TimestepSize, temp);
	}
	else if (this->tOperationMode == "CrNic")
	{
		result.setAll(0.0);

		sg::base::DataVector temp(alpha.getSize());

		applyMassMatrixInner(alpha, temp);
		result.add(temp);

		applyLOperatorInner(alpha, temp);
		result.axpy((-0.5)*this->TimestepSize, temp);
	}
	else if (this->tOperationMode == "AdBas")
	{
		result.setAll(0.0);

		applyMassMatrixInner(alpha, result);
	}
	else if (this->tOperationMode == "BDF2")
	{
		double tDiff=this->TimestepSize/this->TimestepSize_old;
		double alpha0 = (2.0*tDiff+1.0)/(tDiff+1.0);
		result.setAll(0.0);

		sg::base::DataVector temp(alpha.getSize());

		applyMassMatrixInner(alpha, temp);

		temp.mult(alpha0);
		result.add(temp);

		applyLOperatorInner(alpha, temp);
		result.axpy((-1.0)*this->TimestepSize, temp);
	}
	else if (this->tOperationMode == "F23")
	{
		result.setAll(0.0);
		double tDiff=this->TimestepSize/this->TimestepSize_old;
		double alpha0 = 1.0/(1.0+tDiff);

		applyMassMatrixInner(alpha, result);
		result.mult(alpha0);

	}
	else
	{
		throw new sg::base::algorithm_exception("OperationParabolicPDESolverSystem::mult : An unknown operation mode was specified!");
	}
}

sg::base::DataVector* OperationParabolicPDESolverSystemDirichlet::generateRHS()
{
	sg::base::DataVector rhs_complete(this->alpha_complete->getSize());

	if (this->tOperationMode == "ExEul")
	{
		rhs_complete.setAll(0.0);

		sg::base::DataVector temp(this->alpha_complete->getSize());

		applyMassMatrixComplete(*this->alpha_complete, temp);
		rhs_complete.add(temp);

		applyLOperatorComplete(*this->alpha_complete, temp);
		rhs_complete.axpy(this->TimestepSize, temp);
	}
	else if (this->tOperationMode == "ImEul")
	{
		rhs_complete.setAll(0.0);

		applyMassMatrixComplete(*this->alpha_complete, rhs_complete);
	}
	else if (this->tOperationMode == "CrNic")
	{
		rhs_complete.setAll(0.0);

		sg::base::DataVector temp(this->alpha_complete->getSize());

		applyMassMatrixComplete(*this->alpha_complete, temp);
		rhs_complete.add(temp);

		applyLOperatorComplete(*this->alpha_complete, temp);
		rhs_complete.axpy((0.5)*this->TimestepSize, temp);
	}
	else if (this->tOperationMode == "AdBas")
	{
		rhs_complete.setAll(0.0);

		sg::base::DataVector temp(this->alpha_complete->getSize());

		applyMassMatrixComplete(*this->alpha_complete, temp);
		rhs_complete.add(temp);

		applyLOperatorComplete(*this->alpha_complete, temp);

		temp.mult((2.0)+this->TimestepSize/this->TimestepSize_old);

		sg::base::DataVector temp_old(this->alpha_complete->getSize());
		applyMassMatrixComplete(*this->alpha_complete_old, temp_old);
		applyLOperatorComplete(*this->alpha_complete_old, temp_old);
		temp_old.mult(this->TimestepSize/this->TimestepSize_old);
		temp.sub(temp_old);

		rhs_complete.axpy((0.5)*this->TimestepSize, temp);
	}
	else if (this->tOperationMode == "BDF2")
	{
		rhs_complete.setAll(0.0);

		sg::base::DataVector temp(this->alpha_complete->getSize());

		applyMassMatrixComplete(*this->alpha_complete, temp);

		double tDiff = this->TimestepSize/this->TimestepSize_old;

		double alpha1 = tDiff +1.0;
		temp.mult(alpha1);
		rhs_complete.add(temp);

		sg::base::DataVector temp_old(this->alpha_complete->getSize());
		applyMassMatrixComplete(*this->alpha_complete_old, temp_old);

		double alpha2 = tDiff*tDiff/(1.0+tDiff);
		temp_old.mult(alpha2);
		rhs_complete.sub(temp_old);
	}
	else if (this->tOperationMode == "F23")
	{
		rhs_complete.setAll(0.0);
		double tDiff = this->TimestepSize/this->TimestepSize_old;
		double alpha0 = (1.0+tDiff);
		double alpha1 = alpha0*(tDiff -1.0);
		double alpha2 = -alpha0*(tDiff*tDiff/(tDiff+1.0));


		sg::base::DataVector temp(this->alpha_complete->getSize());
		sg::base::DataVector temp_old(this->alpha_complete->getSize());

		applyMassMatrixComplete(*this->alpha_complete, temp);
		temp.mult(alpha1);
		rhs_complete.sub(temp);

		applyLOperatorComplete(*this->alpha_complete, temp);
		temp.mult(alpha0*this->TimestepSize);

		applyMassMatrixComplete(*this->alpha_complete_old, temp_old);
		temp_old.mult(alpha2);
		rhs_complete.sub(temp_old);

		rhs_complete.add(temp);
	}
	else
	{
		throw new sg::base::algorithm_exception("OperationParabolicPDESolverSystem::generateRHS : An unknown operation mode was specified!");
	}

	// Now we have the right hand side, lets apply the riskfree rate for the next timestep
	this->startTimestep();

	// Now apply the boundary ansatzfunctions to the inner ansatzfunctions
	sg::base::DataVector result_complete(this->alpha_complete->getSize());
	sg::base::DataVector alpha_bound(*this->alpha_complete);

	result_complete.setAll(0.0);

	this->BoundaryUpdate->setInnerPointsToZero(alpha_bound);

	// apply CG Matrix
	if (this->tOperationMode == "ExEul")
	{
		applyMassMatrixComplete(alpha_bound, result_complete);
	}
	else if (this->tOperationMode == "ImEul")
	{
		sg::base::DataVector temp(alpha_bound.getSize());

		applyMassMatrixComplete(alpha_bound, temp);
		result_complete.add(temp);

		applyLOperatorComplete(alpha_bound, temp);
		result_complete.axpy((-1.0)*this->TimestepSize, temp);
	}
	else if (this->tOperationMode == "CrNic")
	{
		sg::base::DataVector temp(alpha_bound.getSize());

		applyMassMatrixComplete(alpha_bound, temp);
		result_complete.add(temp);

		applyLOperatorComplete(alpha_bound, temp);
		result_complete.axpy((-0.5)*this->TimestepSize, temp);
	}
	else if (this->tOperationMode == "AdBas")
	{
		applyMassMatrixComplete(alpha_bound, result_complete);
	}
	else if (this->tOperationMode == "BDF2")
	{
		double tDiff =this->TimestepSize/this->TimestepSize_old;
		double alpha0 = (2.0*tDiff+1.0)/(tDiff+1.0);
		sg::base::DataVector temp(alpha_bound.getSize());
		applyMassMatrixComplete(alpha_bound, temp);
		temp.mult(alpha0);
		result_complete.add(temp);

		applyLOperatorComplete(alpha_bound, temp);
		result_complete.axpy((-1.0)*this->TimestepSize, temp);

	}
	else if (this->tOperationMode == "F23")
	{

		double tDiff=this->TimestepSize/this->TimestepSize_old;
		double alpha0 = 1.0/(1.0+tDiff);

		applyMassMatrixComplete(alpha_bound, result_complete);
		result_complete.mult(alpha0);

	}
	else
	{
		throw new sg::base::algorithm_exception("OperationParabolicPDESolverSystem::generateRHS : An unknown operation mode was specified!");
	}
	rhs_complete.sub(result_complete);

	if (this->rhs != NULL)
	{
		delete this->rhs;
	}

	this->rhs = new sg::base::DataVector(this->alpha_inner->getSize());
	this->GridConverter->calcInnerCoefs(rhs_complete, *this->rhs);

	return this->rhs;
}

sg::base::DataVector* OperationParabolicPDESolverSystemDirichlet::getGridCoefficientsForCG()
{
	this->GridConverter->calcInnerCoefs(*this->alpha_complete, *this->alpha_inner);

	return this->alpha_inner;
}

size_t OperationParabolicPDESolverSystemDirichlet::getInnerMatrix(std::string& mtxString)
{
	size_t vector_size = this->InnerGrid->getSize();

	sg::base::DataVector alpha(vector_size);
	sg::base::DataVector result(vector_size);
	std::stringstream mtxStream;
	std::stringstream mtxHeaderStream;
	size_t nonZeros = 0;

	mtxHeaderStream.clear();
	mtxStream.clear();

	// loop over the system matrices columns
	for (size_t i = 0; i < vector_size; i++)
	{
		alpha.setAll(0.0);
		result.setAll(0.0);
		alpha.set(i, 1.0);

		// calculate column via Up/Down
		mult(alpha, result);

		// serialize result into mtxStream
		for (size_t j = 0; j < vector_size; j++)
		{
			if (result[j] != 0.0)
			{
				mtxStream << (j+1) << " " << (i+1) << " " << std::scientific << result[j] << std::endl;
				nonZeros++;
			}
		}
	}

	// Generate Header Line
	mtxHeaderStream << "%%MatrixMarket matrix coordinate real general" << std::endl << vector_size << " " << vector_size << " " << nonZeros << std::endl;

	mtxString = mtxHeaderStream.str() + mtxStream.str();

	return nonZeros;
}

void OperationParabolicPDESolverSystemDirichlet::getInnerMatrixDiagonal(std::string& mtxString)
{
	size_t vector_size = this->InnerGrid->getSize();

	sg::base::DataVector alpha(vector_size);
	sg::base::DataVector result(vector_size);
	std::stringstream mtxStream;
	std::stringstream mtxHeaderStream;

	mtxHeaderStream.clear();
	mtxStream.clear();

	// loop over the system matrices columns
	for (size_t i = 0; i < vector_size; i++)
	{
		alpha.setAll(0.0);
		result.setAll(0.0);
		alpha.set(i, 1.0);

		// calculate column via Up/Down
		mult(alpha, result);

		// serialize result into mtxStream
		mtxStream << (i+1) << " " << (i+1) << " " << std::scientific << result[i] << std::endl;
	}

	// Generate Header Line
	mtxHeaderStream << "%%MatrixMarket matrix coordinate real general" << std::endl << vector_size << " " << vector_size << " " << vector_size << std::endl;

	mtxString = mtxHeaderStream.str() + mtxStream.str();
}

void OperationParabolicPDESolverSystemDirichlet::getInnerMatrixDiagonalRowSum(std::string& mtxString)
{
	size_t vector_size = this->InnerGrid->getSize();

	sg::base::DataVector alpha(vector_size);
	sg::base::DataVector result(vector_size);
	sg::base::DataVector sum(vector_size);
	std::stringstream mtxStream;
	std::stringstream mtxHeaderStream;

	mtxHeaderStream.clear();
	mtxStream.clear();
	sum.setAll(0.0);

	// loop over the system matrices columns
	for (size_t i = 0; i < vector_size; i++)
	{
		alpha.setAll(0.0);
		result.setAll(0.0);
		alpha.set(i, 1.0);

		// calculate column via Up/Down
		mult(alpha, result);

		// serialize result into mtxStream
		sum.add(result);
	}

	for (size_t i = 0; i < vector_size; i++)
	{
		// serialize result into mtxStream
		mtxStream << (i+1) << " " << (i+1) << " " << std::scientific << sum[i] << std::endl;
	}

	// Generate Header Line
	mtxHeaderStream << "%%MatrixMarket matrix coordinate real general" << std::endl << vector_size << " " << vector_size << " " << vector_size << std::endl;

	mtxString = mtxHeaderStream.str() + mtxStream.str();
}

}
}