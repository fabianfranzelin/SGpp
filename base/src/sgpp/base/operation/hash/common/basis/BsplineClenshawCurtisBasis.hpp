// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#ifndef BSPLINE_CLENSHAW_CURTIS_BASE_HPP
#define BSPLINE_CLENSHAW_CURTIS_BASE_HPP

#include <cmath>
#include <sgpp/base/operation/hash/common/basis/Basis.hpp>
#include <sgpp/base/tools/ClenshawCurtisTable.hpp>

#include <sgpp/globaldef.hpp>

namespace SGPP {
  namespace base {

    /**
     * B-spline basis on Clenshaw-Curtis grids.
     */
    template<class LT, class IT>
    class BsplineClenshawCurtisBasis: public Basis<LT, IT> {
      protected:
        /// B-spline basis for B-spline evaluation
        BsplineBasis<LT, IT> bsplineBasis;
        std::vector<double> knots;

      public:
        /**
         * Default constructor.
         */
        BsplineClenshawCurtisBasis()
          : bsplineBasis(BsplineBasis<LT, IT>()) {
        }

        /**
         * Constructor.
         *
         * @param degree        B-spline degree, must be odd (if it's even, degree - 1 is used)
         */
        BsplineClenshawCurtisBasis(size_t degree)
          : bsplineBasis(BsplineBasis<LT, IT>(degree)),
            knots(std::vector<double>(degree + 2, 0.0)) {
        }

        /**
         * @param x     evaluation point
         * @param p     B-spline degree
         * @param k     index of B-spline in the knot sequence
         * @param xi    knot sequence
         * @return      value of non-uniform B-spline with knots
         *              \f$\{\xi_k, ... \xi_{k+p+1}\}\f$
         */
        inline double nonUniformBSpline(double x, size_t p, size_t k,
                                        const std::vector<double>& xi) const {
          if (p == 0) {
            // characteristic function of [xi[k], xi[k+1])
            return (((x >= xi[k]) && (x < xi[k + 1])) ? 1.0 : 0.0);
          } else if ((x < xi[k]) || (x >= xi[k + p + 1])) {
            // out of support
            return 0.0;
          } else {
            // Cox-de-Boor recursion
            return (x - xi[k]) / (xi[k + p] - xi[k])
                   * nonUniformBSpline(x, p - 1, k, xi)
                   + (1.0 - (x - xi[k + 1]) / (xi[k + p + 1] - xi[k + 1]))
                   * nonUniformBSpline(x, p - 1, k + 1, xi);
          }
        }

        /**
         * @param x     evaluation point
         * @param p     B-spline degree
         * @param k     index of B-spline in the knot sequence
         * @param xi    knot sequence
         * @return      value of derivative of non-uniform B-spline with knots
         *              \f$\{\xi_k, ... \xi_{k+p+1}\}\f$
         */
        inline double nonUniformBSplineDx(double x, size_t p, size_t k,
                                          const std::vector<double>& xi) const {
          if (p == 0) {
            return 0.0;
          } else if ((x < xi[k]) || (x >= xi[k + p + 1])) {
            return 0.0;
          } else {
            const double pDbl = static_cast<double>(p);

            return pDbl / (xi[k + p] - xi[k]) * nonUniformBSpline(x, p - 1, k, xi)
                   - pDbl / (xi[k + p + 1] - xi[k + 1])
                   * nonUniformBSpline(x, p - 1, k + 1, xi);
          }
        }

        /**
         * @param x     evaluation point
         * @param p     B-spline degree
         * @param k     index of B-spline in the knot sequence
         * @param xi    knot sequence
         * @return      value of 2nd derivative of non-uniform B-spline with knots
         *              \f$\{\xi_k, ... \xi_{k+p+1}\}\f$
         */
        inline double nonUniformBSplineDxDx(double x, size_t p, size_t k,
                                            const std::vector<double>& xi) const {
          if (p <= 1) {
            return 0.0;
          } else if ((x < xi[k]) || (x >= xi[k + p + 1])) {
            return 0.0;
          } else {
            const double pDbl = static_cast<double>(p);
            const double alphaKP = pDbl / (xi[k + p] - xi[k]);
            const double alphaKp1P = pDbl / (xi[k + p + 1] - xi[k + 1]);
            const double alphaKPm1 = (pDbl - 1.0) / (xi[k + p - 1] - xi[k]);
            const double alphaKp1Pm1 = (pDbl - 1.0) / (xi[k + p] - xi[k + 1]);
            const double alphaKp2Pm1 = (pDbl - 1.0) / (xi[k + p + 1] - xi[k + 2]);

            return alphaKP * alphaKPm1 * nonUniformBSpline(x, p - 2, k, xi)
                   - (alphaKP + alphaKp1P) * alphaKp1Pm1
                   * nonUniformBSpline(x, p - 2, k + 1, xi)
                   + alphaKp1P * alphaKp2Pm1 * nonUniformBSpline(x, p - 2, k + 2, xi);
          }
        }

        /**
         * @param l     level of the grid point
         * @param i     index of the grid point
         * @return      i-th Clenshaw-Curtis grid point with level l
         */
        inline double clenshawCurtisPoint(LT l, IT i) const {
          return clenshawCurtisTable.getPoint(l, i);
        }

        /**
         * Construct the (p+2) Clenshaw-Curtis knots of a B-spline basis function
         * and save them in knots.
         *
         * @param l     level of basis function
         * @param i     index of basis function
         */
        inline void constructKnots(LT l, IT i) {
          const IT hInv = static_cast<IT>(1) << l;
          const size_t p = bsplineBasis.getDegree();

          knots[(p + 1) / 2] = clenshawCurtisTable.getPoint(l, i);

          if (i < (p + 1) / 2) {
            // grid point index is too far on the left
            // ==> extrapolate grid points linearly
            size_t a = (p + 1) / 2 - i;

            for (size_t j = a; j < (p + 1) / 2; j++) {
              knots[j] = clenshawCurtisTable.getPoint(l, static_cast<IT>(j - a));
            }

            double h = knots[a + 1] - knots[a];

            // equivalent to "for (int j = a-1; j >= 0; j--)"
            for (size_t j = a; j-- > 0;) {
              knots[j] = knots[j + 1] - h;
            }
          } else {
            // all grid points on the left can be calculated
            for (size_t j = 0; j < (p + 1) / 2; j++) {
              knots[j] = clenshawCurtisTable.getPoint(
                           l, static_cast<IT>(i - (p + 1) / 2 + j));
            }
          }

          if (i + (p + 1) / 2 > hInv) {
            // grid point index is too far on the right
            // ==> extrapolate grid points linearly
            size_t b = hInv + (p + 1) / 2 - i;

            for (size_t j = (p + 1) / 2 + 1; j <= b; j++) {
              knots[j] = clenshawCurtisTable.getPoint(
                           l, static_cast<IT>(i - (p + 1) / 2 + j));
            }

            double h = knots[b] - knots[b - 1];

            for (size_t j = b + 1; j < p + 2; j++) {
              knots[j] = knots[j - 1] + h;
            }
          } else {
            // all grid points on the right can be calculated
            for (size_t j = (p + 1) / 2 + 1; j < p + 2; j++) {
              knots[j] = clenshawCurtisTable.getPoint(
                           l, static_cast<IT>(i - (p + 1) / 2 + j));
            }
          }
        }

        /**
         * @param l     level of basis function
         * @param i     index of basis function
         * @param x     evaluation point
         * @return      value of Clenshaw-Curtis B-spline basis function
         */
        inline double eval(LT l, IT i, double x) {
          if (l == 0) {
            return bsplineBasis.uniformBSpline(
                     x - static_cast<double>(i)
                     + static_cast<double>(bsplineBasis.getDegree() + 1) / 2.0,
                     bsplineBasis.getDegree());
          } else {
            constructKnots(l, i);
            return nonUniformBSpline(x, bsplineBasis.getDegree(), 0, knots);
          }
        }

        /**
         * @param l     level of basis function
         * @param i     index of basis function
         * @param x     evaluation point
         * @return      value of derivative of Clenshaw-Curtis B-spline basis function
         */
        inline double evalDx(LT l, IT i, double x) {
          if (l == 0) {
            return bsplineBasis.uniformBSplineDx(
                     x - static_cast<double>(i)
                     + static_cast<double>(bsplineBasis.getDegree() + 1) / 2.0,
                     bsplineBasis.getDegree());
          } else {
            constructKnots(l, i);
            return nonUniformBSplineDx(x, bsplineBasis.getDegree(), 0, knots);
          }
        }

        /**
         * @param l     level of basis function
         * @param i     index of basis function
         * @param x     evaluation point
         * @return      value of 2nd derivative of Clenshaw-Curtis B-spline basis function
         */
        inline double evalDxDx(LT l, IT i, double x) {
          if (l == 0) {
            return bsplineBasis.uniformBSplineDxDx(
                     x - static_cast<double>(i)
                     + static_cast<double>(bsplineBasis.getDegree() + 1) / 2.0,
                     bsplineBasis.getDegree());
          } else {
            constructKnots(l, i);
            return nonUniformBSplineDxDx(x, bsplineBasis.getDegree(), 0, knots);
          }
        }

        /**
         * @return      B-spline degree
         */
        inline size_t getDegree() const {
          return bsplineBasis.getDegree();
        }
    };

    // default type-def (unsigned int for level and index)
    typedef BsplineBoundaryBasis<unsigned int, unsigned int> SBsplineBoundaryBase;
  }
}

#endif /* BSPLINE_CLENSHAW_CURTIS_BASE_HPP */
