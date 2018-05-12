// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/globaldef.hpp>

#include <sgpp/datadriven/application/LearnerSGDEOnOff.hpp>
#include <sgpp/datadriven/algorithm/DBMatOffline.hpp>
#include <sgpp/datadriven/algorithm/DBMatOfflineFactory.hpp>
#include <sgpp/datadriven/configuration/DensityEstimationConfiguration.hpp>
#include <sgpp/datadriven/configuration/RegularizationConfiguration.hpp>
#include <sgpp/datadriven/tools/ARFFTools.hpp>

#include <string>
#include <vector>

using sgpp::base::DataMatrix;
using sgpp::base::DataVector;


/**
 * Returns the interactions of the DirectNeighbour Stencil
 * 
 * @param res number of pixels in each dimension. The total image size is res*res
 * @param return vector of interactions
 */
std::vector<std::vector<size_t>> getDirectNeighbours(size_t res) {
  size_t geodim = res;
  std::vector<std::vector<size_t>> vec = std::vector<std::vector<size_t>>();

  for (size_t i = 0; i < geodim; i++) {
    for (size_t j = 0; j < geodim-1; j++) {
      std::vector<size_t> xdir = std::vector<size_t>();

      xdir.push_back(i*geodim+j);

      xdir.push_back(i*geodim+j+1);

      vec.push_back(xdir);
    }
  }
  for (size_t i = 0; i < geodim-1; i++) {
    for (size_t j = 0; j < geodim; j++) {
      std::vector<size_t> ydir = std::vector<size_t>();

      ydir.push_back(i*geodim+j);

      ydir.push_back((i+1)*geodim+j);

      vec.push_back(ydir);
    }
  }
  // 1d vector for all dimensions
  for (size_t i = 0; i < geodim*geodim; i++) {
    std::vector<size_t> tmp = std::vector<size_t>();
    tmp.push_back(i);
    vec.push_back(tmp);
  }
  // add empty vector
  std::vector<size_t> empty = std::vector<size_t>();
  vec.push_back(empty);


  return vec;
}

std::vector<std::vector<size_t>> getConvs(size_t res) {
  size_t geodim = res;
  std::vector<std::vector<size_t>> vec = std::vector<std::vector<size_t>>();

  for (size_t i = 0; i < geodim-1; i+=2) {
    for (size_t j = 0; j < geodim-1; j+=2) {
      std::vector<size_t> xdir1 = std::vector<size_t>();
      std::vector<size_t> xdir2 = std::vector<size_t>();
      std::vector<size_t> ydir1 = std::vector<size_t>();
      std::vector<size_t> ydir2 = std::vector<size_t>();

      xdir1.push_back(i*geodim+j);
      ydir1.push_back(i*geodim+j);

      xdir1.push_back(i*geodim+j+1);
      ydir2.push_back(i*geodim+j+1);

      xdir2.push_back((i+1)*geodim+j);
      ydir1.push_back((i+1)*geodim+j);

      xdir2.push_back((i+1)*geodim+j+1);
      ydir2.push_back((i+1)*geodim+j+1);


      vec.push_back(xdir1);
      vec.push_back(xdir2);
      vec.push_back(ydir1);
      vec.push_back(ydir2);
    }
  }

  for (size_t i = 0; i < geodim*geodim; i++) {
    std::vector<size_t> tmp = std::vector<size_t>();
    tmp.push_back(i);
    vec.push_back(tmp);
  }
  std::vector<size_t> empty = std::vector<size_t>();
  vec.push_back(empty);


  return vec;
}

/**
 * This example shows how to build and decompose a matrix for
 * On/Offline SGDE when using interaction Terms
 * 
 * The OnOffSGDE can read the matrix file generated by this example
 * Some parameters set in this example can be overwritten by the
 * DensityEstimation Class
 */

int main() {
  int lvl = 3;
  for (size_t res = 28; res <= 28; res+=2) {
    std::string filename = "mats/" + std::to_string(res) + "x" + std::to_string(res)
       + "_ModLin_NN_Inter_lvl"+std::to_string(lvl)+"_Chol.out";
    std::cout << "Setting up " << filename << std::endl;
  /**
  * The grid configuration.
  */
  std::cout << "# create grid config" << std::endl;
  sgpp::base::RegularGridConfiguration gridConfig;
  gridConfig.dim_ = res*res;
  gridConfig.level_ = lvl;
  // gridConfig.type_ = sgpp::base::GridType::Linear;
  gridConfig.type_ = sgpp::base::GridType::ModLinear;

  /**
  * Configure regularization.
  */
  std::cout << "# create regularization config" << std::endl;
  sgpp::datadriven::RegularizationConfiguration regularizationConfig;
  regularizationConfig.type_ = sgpp::datadriven::RegularizationType::Identity;
  // initial regularization parameter lambda
  regularizationConfig.lambda_ = 0.01;

  /**
  * Select the desired decomposition type for the offline step.
  * Note: Refinement/Coarsening only possible for Cholesky decomposition.
  */
  sgpp::datadriven::MatrixDecompositionType dt;
  std::string decompType;
  // choose "LU decomposition"
  // dt = MatrixDecompositionType::DBMatDecompLU;
  // decompType = "LU decomposition";
  // choose"Eigen decomposition"
  // dt = MatrixDecompositionType::DBMatDecompEigen;
  // decompType = "Eigen decomposition";
  // choose "Cholesky decomposition"
  dt = sgpp::datadriven::MatrixDecompositionType::Chol;
  decompType = "Cholesky decomposition";
  //      dt = sgpp::datadriven::MatrixDecompositionType::IChol;
  //      decompType = "Incomplete Cholesky decomposition";
  //    dt = sgpp::datadriven::MatrixDecompositionType::DenseIchol;
  //    decompType = "Incomplete Cholesky decomposition on Dense Matrix";
  std::cout << "Decomposition type: " << decompType << std::endl;
  sgpp::datadriven::DensityEstimationConfiguration densityEstimationConfig;
  densityEstimationConfig.decomposition_ = dt;

  /**
  * Configure adaptive refinement (if Cholesky is chosen). As refinement
  * monitor the periodic monitor or the convergence monitor
  * can be chosen. Possible refinement indicators are
  * surplus refinement, data-based refinement, zero-crossings-based
  * refinement.
  */
  std::cout << "# create adaptive refinement configuration" << std::endl;
  std::string refMonitor;
  // select periodic monitor - perform refinements in fixed intervals
  refMonitor = "periodic";
  std::cout << "Refinement monitor: " << refMonitor << std::endl;
  std::string refType;
  // select surplus refinement
  // refType = "surplus";
  // select data-based refinement
  // refType = "data";
  // select zero-crossings-based refinement
  refType = "zero";
  std::cout << "Refinement type: " << refType << std::endl;
  sgpp::base::AdpativityConfiguration adaptConfig;
  /**
  * Specify number of refinement steps and the max number
  * of grid points to refine each step.
  */
  adaptConfig.numRefinements_ = 0;
  adaptConfig.noPoints_ = 7;
  adaptConfig.threshold_ = 0.0;  // only required for surplus refinement

  std::unique_ptr<sgpp::base::Grid> grid;
    if (gridConfig.type_ == sgpp::base::GridType::ModLinear) {
      grid =
          std::unique_ptr<sgpp::base::Grid>{sgpp::base::Grid::createModLinearGrid(gridConfig.dim_)};
    } else if (gridConfig.type_ == sgpp::base::GridType::Linear) {
      grid = std::unique_ptr<sgpp::base::Grid>{sgpp::base::Grid::createLinearGrid(gridConfig.dim_)};
    } else {
      return 1;
    }

  sgpp::datadriven::DBMatOffline *offline =
    sgpp::datadriven::DBMatOfflineFactory::buildOfflineObject(gridConfig,
                                                              adaptConfig,
                                                              regularizationConfig,
                                                              densityEstimationConfig);
  offline->interactions = getDirectNeighbours(res);
  std::cout << "Building Matrix..." << std::endl;
  offline->buildMatrix(&(*grid), regularizationConfig);
  std::cout << "Matrix build.\nBegin decomposition..." << std::endl;
  offline->decomposeMatrix(regularizationConfig, densityEstimationConfig);
  // offline->printMatrix();
    offline->store(filename);
  }
}
