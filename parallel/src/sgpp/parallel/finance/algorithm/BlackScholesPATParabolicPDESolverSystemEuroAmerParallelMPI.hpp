// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#ifndef BLACKSCHOLESPATPARABOLICPDESOLVERSYSTEMEUROAMERPARALLELMPI_HPP
#define BLACKSCHOLESPATPARABOLICPDESOLVERSYSTEMEUROAMERPARALLELMPI_HPP

#include <sgpp/finance/algorithm/BlackScholesPATParabolicPDESolverSystemEuroAmer.hpp>

#include <sgpp/globaldef.hpp>
#include <string>

namespace sgpp {
namespace parallel {

/**
 * This class implements the ParabolicPDESolverSystem for the BlackScholes
 * Equation.
 *
 * Here a European Option with fix Dirichlet boundaries is solved.
 *
 * It's derived from the existing BlackScholesParabolicPDESolverSystemEuropean but uses
 * the OMP task concept to enable further parallelization possibilities
 * in the calculation of the space-discretization operator (L)
 *
 * Parallelization of the FEM operators is done by using MPI.
 *
 * Here a transformation to the Heat Equation was done -> reduces computational effort.
 */
class BlackScholesPATParabolicPDESolverSystemEuroAmerParallelMPI
    : public sgpp::finance::BlackScholesPATParabolicPDESolverSystemEuroAmer {
 protected:
  virtual void applyLOperatorInner(sgpp::base::DataVector& alpha, sgpp::base::DataVector& result);

  virtual void applyLOperatorComplete(sgpp::base::DataVector& alpha,
                                      sgpp::base::DataVector& result);

  virtual void applyMassMatrixInner(sgpp::base::DataVector& alpha, sgpp::base::DataVector& result);

  virtual void applyMassMatrixComplete(sgpp::base::DataVector& alpha,
                                       sgpp::base::DataVector& result);

  // right-hand side corrector
  sgpp::base::DataVector* rhs_corrector;

 public:
  /**
   * Std-Constructor
   *
   * @param SparseGrid reference to the sparse grid
   * @param alpha the ansatzfunctions' coefficients
   * @param lambda reference to the mus
   * @param eigenvecs reference to the eigenvectors of the co-variance matrix
   * @param mu_hat reference to transformed drifts and correlation, needed for constraint of
   * American options
   * @param TimestepSize the size of one timestep used in the ODE Solver
   * @param OperationMode specifies in which solver this matrix is used, valid values are: ExEul for
   * explicit Euler,
   *                ImEul for implicit Euler, CrNic for Crank Nicolson solver
   * @param dStrike the strike of the option
   * @param option_type the option to be solved std_amer_put as an special case
   * @param r the riskfree rate
   * @param useCoarsen specifies if the grid should be coarsened between timesteps
   * @param coarsenThreshold Threshold to decide, if a grid point should be deleted
   * @param adaptSolveMode adaptive mode during solving: coarsen, refine, coarsenNrefine
   * @param numCoarsenPoints number of point that should be coarsened in one coarsening step
   * !CURRENTLY UNUSED PARAMETER!
   * @param refineThreshold Threshold to decide, if a grid point should be refined
   * @param refineMode refineMode during solving Black Scholes Equation: classic or maxLevel
   * @param refineMaxLevel max. level for refinement during solving
   */
  BlackScholesPATParabolicPDESolverSystemEuroAmerParallelMPI(
      sgpp::base::Grid& SparseGrid, sgpp::base::DataVector& alpha, sgpp::base::DataVector& lambda,
      sgpp::base::DataMatrix& eigenvecs, sgpp::base::DataVector& mu_hat, double TimestepSize,
      std::string OperationMode, double dStrike, std::string option_type, double r,
      bool useCoarsen = false, double coarsenThreshold = 0.0, std::string adaptSolveMode = "none",
      int numCoarsenPoints = -1, double refineThreshold = 0.0, std::string refineMode = "classic",
      sgpp::base::GridIndex::level_type refineMaxLevel = 0);

  /**
   * Std-Destructor
   */
  virtual ~BlackScholesPATParabolicPDESolverSystemEuroAmerParallelMPI();

  /**
   * Multiplicates a vector with the matrix, parallel
   *
   * @param alpha DataVector that contains the ansatzfunctions' coefficients
   * @param result DataVector into which the result of the space discretization operation is stored
   */
  virtual void mult(sgpp::base::DataVector& alpha, sgpp::base::DataVector& result);

  /**
   * generates the right hand side of the system, parallel
   *
   * @return returns the rhs
   */
  virtual sgpp::base::DataVector* generateRHS();

  void finishTimestep(bool isLastTimestep = false);
};
}  // namespace parallel
}  // namespace sgpp

#endif /* BLACKSCHOLESPATPARABOLICPDESOLVERSYSTEMEUROAMERPARALLELMPI_HPP */