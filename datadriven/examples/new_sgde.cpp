// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/base/grid/Grid.hpp>
#include <sgpp/base/operation/BaseOpFactory.hpp>
#include <sgpp/datadriven/DatadrivenOpFactory.hpp>
#include <sgpp/datadriven/algorithm/DensitySystemMatrix.hpp>
#include <sgpp/datadriven/application/LearnerSGDE.hpp>
#include <sgpp/datadriven/application/RegularizationConfiguration.hpp>
#include <sgpp/globaldef.hpp>
#include <sgpp/pde/operation/PdeOpFactory.hpp>
#include <sgpp/solver/TypesSolver.hpp>
#include <sgpp_optimization.hpp>

#include <functional>
#include <random>
#include <vector>
#ifdef USE_CGAL
#include <CGAL/basic.h>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/QP_solution.h>
#include <CGAL/MP_Float.h>
typedef CGAL::MP_Float ET;
typedef CGAL::Quadratic_program_solution<ET> Solution;
typedef CGAL::Quadratic_program_from_iterators<
 double**,                                           //for A
 double*,                                                 // for b
 CGAL::Const_oneset_iterator<CGAL::Comparison_result>, // for r
 bool*,                                                // for fl
 double*,                                                 // for l
 bool*,                                                // for fu
 double*,                                                 // for u
 double**,                                                // for D
 double*>                                                 // for c
Program;
#endif

using sgpp::base::DataMatrix;
using sgpp::base::DataVector;
using sgpp::base::Grid;
using sgpp::base::GridPoint;
using sgpp::base::GridStorage;
using sgpp::base::GridType;
using sgpp::base::OperationEval;
using sgpp::base::OperationMatrix;
using sgpp::base::OperationMultipleEval;
using sgpp::datadriven::DensitySystemMatrix;
using sgpp::optimization::WrapperScalarFunction;
using sgpp::optimization::WrapperScalarFunctionGradient;
using sgpp::optimization::WrapperVectorFunction;
using sgpp::optimization::WrapperVectorFunctionGradient;
using sgpp::optimization::optimizer::AugmentedLagrangian;
using sgpp::optimization::optimizer::LogBarrier;
using sgpp::optimization::optimizer::SquaredPenalty;

void randu(DataVector& rvar, std::mt19937& generator) {
  std::normal_distribution<double> distribution(0.5, 0.1);
  for (size_t j = 0; j < rvar.getSize(); ++j) {
    rvar[j] = distribution(generator);
    // make sure the sample is in the unit cube
    while (rvar[j] < 0 || rvar[j] > 1) rvar[j] = distribution(generator);
  }
}

void createSamples(DataMatrix& rvar, std::uint64_t seedValue = std::mt19937_64::default_seed) {
  size_t nsamples = rvar.getNrows(), ndim = rvar.getNcols();
  std::mt19937 generator(seedValue);
  DataVector sample(ndim);
  for (size_t i = 0; i < nsamples; ++i) {
    randu(sample, generator);
    rvar.setRow(i, sample);
  }
}

void checkPositive(Grid& grid, const DataVector& alpha) {
  GridStorage* gridStorage = &grid.getStorage();
  size_t storage_size = gridStorage->getSize();
  size_t dims = grid.getDimension();
  GridPoint gp;
  DataVector coords(dims);
  OperationEval* op_eval = sgpp::op_factory::createOperationEval(grid);
  for (size_t i = 0; i < storage_size; i++) {
    gp = gridStorage->getPoint(i);
    gp.getStandardCoordinates(coords);
    // if (op_eval->eval(alpha, coords) < 0) {
    std::cout << "f(" << coords.toString() << ")=" << op_eval->eval(alpha, coords) << std::endl;
    // }
  }
}

#ifdef USE_CGAL
void solve_cgal(DataMatrix& samples, sgpp::base::RegularGridConfiguration& gridConfig, double lambda) {
  // CGAL documentation: https://doc.cgal.org/latest/QP_solver/index.html
  std::unique_ptr<Grid> grid(sgpp::base::Grid::createGrid(gridConfig));
  GridStorage* gridStorage = &(grid->getStorage());
  grid->getGenerator().regular(gridConfig.level_);
  size_t storage_size = gridStorage->getSize();
  std::cout << "storage size:" << storage_size << std::endl;
  size_t numSamples = samples.getNrows();
  size_t dims = samples.getNcols();
  DataMatrix M(storage_size, storage_size);
  DataMatrix C(storage_size, storage_size);
  // loading M matrix
  OperationMatrix* A_op = sgpp::op_factory::createOperationLTwoDotExplicit(&M, *grid);
  OperationMultipleEval* B_op = sgpp::op_factory::createOperationMultipleEval(*grid, samples);
  OperationMatrix* C_op = sgpp::op_factory::createOperationLaplaceExplicit(&C, *grid);
  DataVector b(storage_size);
  DataVector q(storage_size);
  DensitySystemMatrix AlambC(A_op, B_op, C_op, lambda, numSamples);
  AlambC.generateb(b);

  // setting up System matrix: M + lambda*C
  C.mult(lambda);
  M.add(C);

  // Skalaraproduct term of quadratic program: q.T x
  M.mult(b, q);

  // Quadratic program matrix P = M*M.T (M is symmetric)
  double** P_it = new double*[storage_size];
  for (size_t i = 0; i < storage_size; i++) {
    P_it[i] = new double[storage_size];
  }
  // multiplication P = M*M.T
  for (size_t i = 0; i < storage_size; i++) {
    DataVector col(storage_size);
    DataVector tmp(storage_size);
    M.getColumn(i, col);
    M.mult(col, tmp);
    P_it[i] = new double[storage_size];
    for (size_t j = 0; j <= i; j++) {
      P_it[i][j] = tmp.get(j);
      P_it[j][i] = tmp.get(j);
    }
  }
  // ---printing
  for (size_t i = 0; i < storage_size; i++) {
    for (size_t j = 0; j < storage_size; j++) {
      std::cout << P_it[j][i] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "---------------" << std::endl;
  // ---printing

  // getting all grid points for interpolation matrix
  DataMatrix gridPoints(storage_size, dims);
  GridPoint gp;
  for (size_t i = 0; i < storage_size; i++) {
    gp = gridStorage->getPoint(i);
    DataVector coords(dims);
    gp.getStandardCoordinates(coords);
    gridPoints.setRow(i, coords);
  }

  // interpolation matrix (grid point values when multiplied with alpha-vec)
  OperationMultipleEval* G_op = sgpp::op_factory::createOperationMultipleEval(*grid, gridPoints);
  double** G_it = new double*[storage_size];
  for (size_t i = 0; i < storage_size; i++) {
    G_it[i] = new double[storage_size];
  }
  // G_it[i] should contain the i-th COLUMN of G
  for (size_t i = 0; i < storage_size; i++) {
    DataVector result(storage_size);
    DataVector alpha(storage_size, 0.0);
    alpha.set(i, 1.0);
    G_op->mult(alpha, result);
    for (size_t j = 0; j < storage_size; j++) {
      G_it[i][j] = result.get(j);
    }
  }

  for (size_t i = 0; i < storage_size; i++) {
    for (size_t j = 0; j < storage_size; j++) {
      std::cout << G_it[j][i] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << q.toString() << std::endl;
  // constraint relation (i.e. greater than zero)
  CGAL::Const_oneset_iterator<CGAL::Comparison_result> r(CGAL::LARGER);

  // lower upper bounds unused:
  bool* bounded = new bool[storage_size];
  for (size_t i = 0; i < storage_size; i++) {
    bounded[i] = false;
  }
  DataVector bounds(storage_size, 0.0);
  // define the quadratic Programm
  Program qp(static_cast<int>(storage_size),static_cast<int>(storage_size),  // size of problem
             G_it, b.getPointer(), r,     // constraints
             bounded, bounds.getPointer(), bounded, bounds.getPointer(),  // bounds
             P_it, q.getPointer()  // optimization goal
             );
  Solution s = CGAL::solve_quadratic_program(qp, ET());
  Solution::Variable_value_iterator it = s.variable_values_begin();
  DataVector best_alpha(storage_size);
  for (size_t i = 0; i < storage_size; i++) {
    best_alpha.set(i, to_double(*it));
    it++;
  }
  std::cout << "---------------------------" << std::endl;
  std::cout << "best alpha:" << best_alpha.toString() << std::endl;
  std::cout << "objective function:" << to_double(s.objective_value()) << std::endl;
}
#endif

int main(int argc, char** argv) {
  size_t d = 2;
  int level = 3;
  GridType gridType = sgpp::base::GridType::Linear;
  sgpp::base::RegularGridConfiguration gridConfig;
  gridConfig.dim_ = d;
  gridConfig.level_ = level;
  gridConfig.type_ = gridType;
  gridConfig.maxDegree_ = 5;
  DataMatrix trainSamples(1000, 2);
  createSamples(trainSamples, 1234567);
  std::cout << trainSamples.getNrows() << std::endl;
  // solve(trainSamples, gridConfig, lambda);
#ifdef USE_CGAL
  double lambda = 0.0;
  solve_cgal(trainSamples, gridConfig, lambda);
#endif
  return 0;
}
