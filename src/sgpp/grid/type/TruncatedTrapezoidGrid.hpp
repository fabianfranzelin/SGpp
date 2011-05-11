/*
 * TruncatedTrapezoidGrid.hpp
 *
 *  Created on: Aug 4, 2010
 *      Author: aliz
 */

#ifndef TRUNCATEDTRAPEZOIDGRID_HPP_
#define TRUNCATEDTRAPEZOIDGRID_HPP_
#include "grid/Grid.hpp"

#include <iostream>

namespace sg
{
namespace base
{

/**
 * grid with linear base functions with boundaries, pentagon cut
 * Generalization of the LinearBoundary and LinearTrapezoidBoundary Grids
 * The sparse grid does contain all fullgrids with |l|<a given level, and l_min>l_user
 * For l_user=0 we obtain the LinearBoundaryGrid and for l_user=1 we obtain the linear trapezoid boundary grid
 */
class TruncatedTrapezoidGrid : public Grid
{
protected:
	TruncatedTrapezoidGrid(std::istream& istr);

public:
	/**
	 * Constructor Linear Trapezoid Boundary Grid
	 *
	 * @param dim the dimension of the grid
	 */
	TruncatedTrapezoidGrid(size_t dim);

	/**
	 * Constructor Linear Trapezoid Boundary Grid
	 *
	 * @param BB the BoundingBox of the grid
	 */
	TruncatedTrapezoidGrid(BoundingBox& BB);

	/**
	 * Destructor
	 */
	virtual ~TruncatedTrapezoidGrid();

	virtual const char* getType();

	//virtual OperationMultipleEval* createOperationMultipleEval(DataMatrix* dataset){return 0;};
//	virtual OperationMultipleEvalVectorized* createOperationMultipleEvalVectorized(const std::string& VecType, DataMatrix* dataset){return 0;};
//	virtual OperationMultipleEvalVectorizedSP* createOperationMultipleEvalVectorizedSP(const std::string& VecType, DataMatrixSP* dataset){return 0;};
	virtual GridGenerator* createGridGenerator();
	//virtual OperationMatrix* createOperationLaplace(){return 0;};
	//virtual OperationEval* createOperationEval();
	//virtual OperationTest* createOperationTest(){return 0;};
	//virtual OperationHierarchisation* createOperationHierarchisation();
	//virtual OperationMatrix* createOperationLTwoDotProduct(){return 0;};
	//virtual OperationConvert* createOperationConvert();

	// finance operations
	//virtual OperationMatrix* createOperationDelta(DataVector& coef){return 0;};
	//virtual OperationMatrix* createOperationGamma(DataMatrix& coef){return 0;};
	//virtual OperationMatrix* createOperationDeltaLog(DataVector& coef){return 0;};
	//virtual OperationMatrix* createOperationGammaLog(DataMatrix& coef){return 0;};
	/*virtual OperationMatrix* createOperationLB() {return 0;};
	virtual OperationMatrix* createOperationLD() {return 0;};
	virtual OperationMatrix* createOperationLE() {return 0;};
	virtual OperationMatrix* createOperationLF() {return 0;};*/

	static Grid* unserialize(std::istream& istr);
};

}
}

#endif /* TRUNCATEDTRAPEZOIDGRID_HPP_ */
